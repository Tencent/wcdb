/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sqliterk_pager.h"
#include "SQLiteRepairKit.h"
#include "sqliterk_crypto.h"
#include "sqliterk_os.h"
#include "sqliterk_util.h"
#include <errno.h>
#include <string.h>

static int sqliterkPagerParseHeader(sqliterk_pager *pager, int forcePageSize);
static int sqliterkPageAcquireOne(sqliterk_pager *pager,
                                  int pageno,
                                  sqliterk_page **page,
                                  sqliterk_page_type type);

struct sqliterk_page {
    int pageno;
    unsigned char *data; // page data
    sqliterk_page_type type;
};

int sqliterkPagerOpen(const char *path,
                      const sqliterk_cipher_conf *cipher,
                      sqliterk_pager **pager)
{
    // Workaround page size cannot be specified for plain-text
    // databases. For that case, pass non-null cipher_conf with
    // null key and non-zero page size.
    int forcePageSize = 0;
    if (cipher && !cipher->key) {
        forcePageSize = cipher->page_size;
        cipher = NULL;
    }

    if (!pager) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterk_pager *thePager = sqliterkOSMalloc(sizeof(sqliterk_pager));
    if (!thePager) {
        rc = SQLITERK_NOMEM;
        sqliterkOSError(rc, "Not enough memory, required %zu bytes.",
                        sizeof(sqliterk_pager));
        goto sqliterkPagerOpen_Failed;
    }

    rc = sqliterkOSReadOnlyOpen(path, &thePager->file);
    if (rc != SQLITERK_OK) {
        goto sqliterkPagerOpen_Failed;
    }

    if (cipher) {
        // Try KDF salt in SQLite file first.
        sqliterk_cipher_conf c;
        memcpy(&c, cipher, sizeof(c));
        c.kdf_salt = NULL;

        rc = sqliterkCryptoSetCipher(thePager, thePager->file, &c);
        if (rc != SQLITERK_OK)
            goto sqliterkPagerOpen_Failed;

        // Try parsing header.
        sqliterkPagerParseHeader(thePager, 0);

        if (thePager->integrity & SQLITERK_INTEGRITY_HEADER) {
            // If header is parsed successfully, original KDF salt is also correct.
            thePager->integrity |= SQLITERK_INTEGRITY_KDF_SALT;
        } else if (cipher->kdf_salt) {
            // If anything goes wrong, use KDF salt specified in cipher config.
            sqliterkOSWarning(SQLITERK_DAMAGED, "Header cannot be decoded "
                                                "correctly. Trying to apply "
                                                "recovery data.");
            rc = sqliterkCryptoSetCipher(thePager, thePager->file, cipher);
            if (rc != SQLITERK_OK)
                goto sqliterkPagerOpen_Failed;

            rc = sqliterkPagerParseHeader(thePager, 0);
            if (rc != SQLITERK_OK)
                goto sqliterkPagerOpen_Failed;
        }
    } else {
        rc = sqliterkPagerParseHeader(thePager, forcePageSize);
        if (rc != SQLITERK_OK)
            goto sqliterkPagerOpen_Failed;

        // For plain-text databases, just mark KDF salt correct.
        if (thePager->integrity & SQLITERK_INTEGRITY_HEADER)
            thePager->integrity |= SQLITERK_INTEGRITY_KDF_SALT;
    }
    if (!(thePager->integrity & SQLITERK_INTEGRITY_HEADER))
        sqliterkOSWarning(SQLITERK_DAMAGED, "Header corrupted.");
    else
        sqliterkOSInfo(SQLITERK_OK, "Header checksum OK.");

    int pageCount = thePager->pagecount;
    size_t len = sizeof(sqliterk_status) * (pageCount + 1);
    thePager->pagesStatus = sqliterkOSMalloc(len);
    if (!thePager->pagesStatus) {
        rc = SQLITERK_NOMEM;
        sqliterkOSError(rc, "Not enough memory, required %zu bytes.", len);
        goto sqliterkPagerOpen_Failed;
    }

    *pager = thePager;

    return SQLITERK_OK;

sqliterkPagerOpen_Failed:
    if (thePager) {
        sqliterkPagerClose(thePager);
    }
    *pager = NULL;
    return rc;
}

// Get the meta from header and set it into pager.
// For further information, see https://www.sqlite.org/fileformat2.html
static int sqliterkPagerParseHeader(sqliterk_pager *pager, int forcePageSize)
{
    // For encrypted databases, assume default page size, decode the first
    // page, and we have the plain-text header.

    if (!pager) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;

    // Overwrite pager page size if forcePageSize is specified.
    if (forcePageSize) {
        pager->pagesize = forcePageSize;
    }

    size_t size = pager->codec ? pager->pagesize : 100;

    // Read data
    unsigned char *buffer = sqliterkOSMalloc(size);
    if (!buffer) {
        rc = SQLITERK_NOMEM;
        sqliterkOSError(rc, "Not enough memory, required %zu bytes.", size);
        goto sqliterkPagerParseHeader_End;
    }

    rc = sqliterkOSRead(pager->file, 0, buffer, &size);
    if (rc != SQLITERK_OK) {
        if (rc == SQLITERK_SHORT_READ)
            sqliterkOSError(rc, "File truncated.");
        else
            sqliterkOSError(rc, "Cannot read file '%s': %s",
                            sqliterkOSGetFilePath(pager->file),
                            strerror(errno));
        pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
        goto sqliterkPagerParseHeader_End;
    }

    pager->integrity |= SQLITERK_INTEGRITY_HEADER;

    if (pager->codec) {
        rc = sqliterkCryptoDecode(pager->codec, 1, buffer);
        if (rc != SQLITERK_OK) {
            sqliterkOSWarning(SQLITERK_DAMAGED,
                              "Failed to decode page 1, header corrupted.");
            pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
        }
    }

    if (pager->integrity & SQLITERK_INTEGRITY_HEADER) {
        if (memcmp(buffer, "SQLite format 3\000", 16) == 0) {
            //parse pagesize
            int pagesize;
            sqliterkParseInt(buffer, 16, 2, &pagesize);
            if (pager->codec || forcePageSize) {
                // Page size is predefined, check whether it matches the header.
                if (pagesize != pager->pagesize) {
                    sqliterkOSWarning(
                        SQLITERK_DAMAGED,
                        "Invalid page size: %d expected, %d returned.",
                        pager->pagesize, pagesize);
                    pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
                }
            } else if (((pagesize - 1) & pagesize) != 0 || pagesize < 512) {
                // Page size is not predefined and value in the header is invalid,
                // use the default page size.
                sqliterkOSWarning(SQLITERK_DAMAGED,
                                  "Page size field is corrupted. Default page "
                                  "size %d is used",
                                  SQLITRK_CONFIG_DEFAULT_PAGESIZE);
                pager->pagesize = SQLITRK_CONFIG_DEFAULT_PAGESIZE;
                pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
            } else {
                // Page size is not predefined and value in the header is valid,
                // use the value in header.
                pager->pagesize = pagesize;
            }

            // parse free page count
            sqliterkParseInt(buffer, 36, 4, &pager->freepagecount);

            // parse reserved bytes
            int reservedBytes;
            sqliterkParseInt(buffer, 20, 1, &reservedBytes);
            if (pager->codec) {
                if (reservedBytes != pager->reservedBytes) {
                    sqliterkOSWarning(SQLITERK_DAMAGED,
                                      "Reserved bytes field doesn't match. %d "
                                      "expected, %d returned.",
                                      pager->reservedBytes, reservedBytes);
                    pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
                }
            } else if (reservedBytes < 0 || reservedBytes > 255) {
                sqliterkOSWarning(
                    SQLITERK_DAMAGED,
                    "The [reserved bytes] field is corrupted. 0 is used");
                pager->reservedBytes = 0;
                pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
            } else
                pager->reservedBytes = reservedBytes;
        } else {
            // Header is corrupted. Defaults the config
            sqliterkOSWarning(SQLITERK_DAMAGED,
                              "SQLite format magic corrupted.");
            if (!pager->codec) {
                pager->pagesize = SQLITRK_CONFIG_DEFAULT_PAGESIZE;
                pager->reservedBytes = 0;
            }
            pager->freepagecount = 0;
            pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
        }
    }

    // Assign page count
    size_t filesize;
    rc = sqliterkOSFileSize(pager->file, &filesize);
    if (rc != SQLITERK_OK) {
        sqliterkOSError(rc, "Failed to get size of file '%s': %s",
                        sqliterkOSGetFilePath(pager->file), strerror(errno));
        goto sqliterkPagerParseHeader_End;
    }

    pager->pagecount =
        (int) ((filesize + pager->pagesize - 1) / pager->pagesize);
    if (pager->pagecount < 1) {
        rc = SQLITERK_DAMAGED;
        sqliterkOSError(rc, "File truncated.");
        goto sqliterkPagerParseHeader_End;
    }

    // Check free page
    if (pager->freepagecount < 0 || pager->freepagecount > pager->pagecount) {
        sqliterkOSWarning(
            SQLITERK_DAMAGED,
            "The [free page count] field is corrupted. 0 is used");
        pager->freepagecount = 0;
        pager->integrity &= ~SQLITERK_INTEGRITY_HEADER;
    }

    // Assign usableSize
    pager->usableSize = pager->pagesize - pager->reservedBytes;

sqliterkPagerParseHeader_End:
    if (buffer) {
        sqliterkOSFree(buffer);
    }
    return rc;
}

int sqliterkPagerClose(sqliterk_pager *pager)
{
    if (!pager) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    if (pager->file) {
        rc = sqliterkOSClose(pager->file);
        pager->file = NULL;
    }
    if (pager->pagesStatus) {
        sqliterkOSFree(pager->pagesStatus);
        pager->pagesStatus = NULL;
    }
    pager->pagesize = 0;
    pager->pagecount = 0;

    sqliterkCryptoFreeCodec(pager);

    sqliterkOSFree(pager);
    return rc;
}

int sqliterkPagerGetPageCount(sqliterk_pager *pager)
{
    if (!pager) {
        return 0;
    }
    return pager->pagecount;
}

int sqliterkPagerIsPagenoValid(sqliterk_pager *pager, int pageno)
{
    if (!pager || pageno < 1 || pageno > pager->pagecount) {
        return SQLITERK_MISUSE;
    }
    return SQLITERK_OK;
}

// Get the page type from file at page [pageno]
int sqliterkPageAcquireType(sqliterk_pager *pager,
                            int pageno,
                            sqliterk_page_type *type)
{
    // TODO: for encrypted databases, decode the whole page.
    // Use sqliterkPageAcquire instead.

    if (!pager || sqliterkPagerIsPagenoValid(pager, pageno) != SQLITERK_OK ||
        !type) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    unsigned char typedata;
    size_t typesize = 1;
    rc = sqliterkOSRead(pager->file,
                        sqliterkPagenoHeaderOffset(pageno) +
                            (pageno - 1) * pager->pagesize,
                        &typedata, &typesize);
    if (rc != SQLITERK_OK) {
        goto sqliterkPageAcquireType_Failed;
    }

    int theType;
    sqliterkParseInt(&typedata, 0, 1, &theType);
    switch (theType) {
        case sqliterk_page_type_interior_index:
        case sqliterk_page_type_interior_table:
        case sqliterk_page_type_leaf_index:
        case sqliterk_page_type_leaf_table:
            *type = theType;
            break;
        default:
            *type = sqliterk_page_type_unknown;
            break;
    }
    return SQLITERK_OK;

sqliterkPageAcquireType_Failed:
    *type = sqliterk_page_type_unknown;
    return rc;
}

// Get whole page data from file at page [pageno] and setup the [page].
int sqliterkPageAcquire(sqliterk_pager *pager, int pageno, sqliterk_page **page)
{
    return sqliterkPageAcquireOne(pager, pageno, page,
                                  sqliterk_page_type_unknown);
}

int sqliterkPageAcquireOverflow(sqliterk_pager *pager,
                                int pageno,
                                sqliterk_page **page)
{
    return sqliterkPageAcquireOne(pager, pageno, page,
                                  sqliterk_page_type_overflow);
}

static int sqliterkPageAcquireOne(sqliterk_pager *pager,
                                  int pageno,
                                  sqliterk_page **page,
                                  sqliterk_page_type type)
{
    if (!pager || !page ||
        sqliterkPagerIsPagenoValid(pager, pageno) != SQLITERK_OK) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterk_page *thePage = sqliterkOSMalloc(sizeof(sqliterk_page));
    if (!thePage) {
        rc = SQLITERK_NOMEM;
        goto sqliterkPageAcquire_Failed;
    }

    thePage->pageno = pageno;

    thePage->data = sqliterkOSMalloc(pager->pagesize);
    if (!thePage->data) {
        rc = SQLITERK_NOMEM;
        goto sqliterkPageAcquire_Failed;
    }

    size_t size = pager->pagesize;
    rc = sqliterkOSRead(pager->file, (pageno - 1) * pager->pagesize,
                        thePage->data, &size);
    if (rc != SQLITERK_OK) {
        goto sqliterkPageAcquire_Failed;
    }

    // For encrypted databases, decode page.
    if (pager->codec) {
        rc = sqliterkCryptoDecode(pager->codec, pageno, thePage->data);
        if (rc != SQLITERK_OK)
            goto sqliterkPageAcquire_Failed;
    }

    // Check type
    if (type == sqliterk_page_type_unknown) {
        sqliterkParseInt(thePage->data, sqliterkPageHeaderOffset(thePage), 1,
                         &type);
        switch (type) {
            case sqliterk_page_type_interior_index:
            case sqliterk_page_type_interior_table:
            case sqliterk_page_type_leaf_index:
            case sqliterk_page_type_leaf_table:
                thePage->type = type;
                break;
            default:
                thePage->type = sqliterk_page_type_unknown;
                break;
        }
    } else {
        thePage->type = type;
    }

    *page = thePage;
    return SQLITERK_OK;

sqliterkPageAcquire_Failed:
    if (thePage) {
        sqliterkPageRelease(thePage);
    }
    *page = NULL;
    return rc;
}

int sqliterkPageRelease(sqliterk_page *page)
{
    if (!page) {
        return SQLITERK_MISUSE;
    }
    if (page->data) {
        sqliterkOSFree(page->data);
        page->data = NULL;
    }
    sqliterkOSFree(page);
    return SQLITERK_OK;
}

// Ahead release the page data to save memory
int sqliterkPageClearData(sqliterk_page *page)
{
    if (!page) {
        return SQLITERK_MISUSE;
    }
    if (page->data) {
        sqliterkOSFree(page->data);
        page->data = NULL;
    }
    return SQLITERK_OK;
}

const unsigned char *sqliterkPageGetData(sqliterk_page *page)
{
    if (!page) {
        return NULL;
    }
    return page->data;
}

int sqliterkPagerGetSize(sqliterk_pager *pager)
{
    if (!pager) {
        return 0;
    }
    return pager->pagesize;
}

int sqliterkPagerGetUsableSize(sqliterk_pager *pager)
{
    if (!pager) {
        return 0;
    }
    return pager->usableSize;
}

int sqliterkPageGetPageno(sqliterk_page *page)
{
    if (!page) {
        return 0;
    }
    return page->pageno;
}
sqliterk_page_type sqliterkPageGetType(sqliterk_page *page)
{
    if (!page) {
        return sqliterk_page_type_unknown;
    }
    return page->type;
}

int sqliterkPagenoHeaderOffset(int pageno)
{
    if (pageno == 1) {
        return 100;
    }
    return 0;
}

int sqliterkPageHeaderOffset(sqliterk_page *page)
{
    if (!page) {
        return 0;
    }
    return sqliterkPagenoHeaderOffset(page->pageno);
}

const char *sqliterkPageGetTypeName(sqliterk_page_type type)
{
    char *name;
    switch (type) {
        case sqliterk_page_type_interior_index:
            name = "interior-index btree";
            break;
        case sqliterk_page_type_interior_table:
            name = "interior-table btree";
            break;
        case sqliterk_page_type_leaf_index:
            name = "leaf-index btree";
            break;
        case sqliterk_page_type_leaf_table:
            name = "leaf-table btree";
            break;
        default:
            name = "unknown page";
            break;
    }
    return name;
}

void sqliterkPagerSetStatus(sqliterk_pager *pager,
                            int pageno,
                            sqliterk_status status)
{
    if (!pager || !pager->pagesStatus ||
        sqliterkPagerIsPagenoValid(pager, pageno) != SQLITERK_OK) {
        return;
    }

    pager->pagesStatus[pageno - 1] = status;
    if (status == sqliterk_status_checked)
        pager->integrity |= SQLITERK_INTEGRITY_DATA;
}

sqliterk_status sqliterkPagerGetStatus(sqliterk_pager *pager, int pageno)
{
    if (!pager || !pager->pagesStatus ||
        sqliterkPagerIsPagenoValid(pager, pageno) != SQLITERK_OK) {
        return sqliterk_status_invalid;
    }
    return pager->pagesStatus[pageno - 1];
}

int sqliterkPagerGetParsedPageCount(sqliterk_pager *pager)
{
    if (!pager || !pager->pagesStatus) {
        return 0;
    }

    int i, count = 0;
    for (i = 0; i < pager->pagecount; i++) {
        if (pager->pagesStatus[i] == sqliterk_status_checked) {
            count++;
        }
    }
    return count;
}

int sqliterkPagerGetValidPageCount(sqliterk_pager *pager)
{
    if (!pager) {
        return 0;
    }
    return pager->pagecount - pager->freepagecount;
}

unsigned int sqliterkPagerGetIntegrity(sqliterk_pager *pager)
{
    if (!pager) {
        return 0;
    }
    return pager->integrity;
}
