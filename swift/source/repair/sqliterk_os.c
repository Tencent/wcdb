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

#include "sqliterk_os.h"
#include "sqliterk_util.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct sqliterk_file {
    char *path;
    int fd;
    int error; // errno will be set when system error occur
};

int sqliterkOSReadOnlyOpen(const char *path, sqliterk_file **file)
{
    if (!path || !path[0] || !file) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterk_file *theFile = sqliterkOSMalloc(sizeof(sqliterk_file));
    if (!theFile) {
        rc = SQLITERK_NOMEM;
        sqliterkOSError(rc, "Not enough memory, required %zu bytes.",
                        sizeof(sqliterk_file));
        goto sqliterkOSReadOnlyOpen_Failed;
    }

    size_t len = sizeof(char) * (strlen(path) + 1);
    theFile->path = sqliterkOSMalloc(len);
    if (!theFile->path) {
        rc = SQLITERK_NOMEM;
        sqliterkOSError(rc, "Not enough memory, required %zu bytes.", len);
        goto sqliterkOSReadOnlyOpen_Failed;
    }
    strncpy(theFile->path, path, len);

    // Open the file in read-only mode, since we do not intend to modify it
    theFile->fd = open(theFile->path, O_RDONLY);
    if (theFile->fd < 0) {
        rc = SQLITERK_CANTOPEN;
        sqliterkOSError(rc, "Cannot open '%s' for reading: %s", theFile->path,
                        strerror(errno));
        goto sqliterkOSReadOnlyOpen_Failed;
    }
    *file = theFile;
    return SQLITERK_OK;

sqliterkOSReadOnlyOpen_Failed:
    if (theFile) {
        sqliterkOSClose(theFile);
    }
    *file = NULL;
    return rc;
}

int sqliterkOSClose(sqliterk_file *file)
{
    if (!file) {
        return SQLITERK_MISUSE;
    }
    if (file->path) {
        sqliterkOSFree((char *) file->path);
        file->path = NULL;
    }
    if (file->fd >= 0) {
        close(file->fd);
        file->fd = -1;
    }
    file->error = 0;
    sqliterkOSFree(file);
    return SQLITERK_OK;
}

int sqliterkOSRead(sqliterk_file *file,
                   off_t offset,
                   unsigned char *data,
                   size_t *size)
{
    if (!file || file->fd < 0) {
        return SQLITERK_MISUSE;
    }
    off_t newOffset = lseek(file->fd, offset, SEEK_SET);
    if (newOffset == -1) {
        file->error = errno;
        return SQLITERK_IOERR;
    }
    size_t left = *size;
    size_t cnt = 0;
    ssize_t got = 0;
    do {
        got = read(file->fd, data, left);
        if (got < 0) {
            if (errno == EINTR) {
                got = 1;
                continue;
            }
            file->error = errno;
            return SQLITERK_IOERR;
        } else if (got > 0) {
            left -= got;
            cnt += got;
            data = data + got;
        }
    } while (got > 0 && left > 0);
    *size = cnt;
    if (left > 0) {
        return SQLITERK_SHORT_READ;
    }
    return SQLITERK_OK;
}

int sqliterkOSFileSize(sqliterk_file *file, size_t *filesize)
{
    if (!file || file->fd < 0) {
        return SQLITERK_MISUSE;
    }
    struct stat statbuf;
    if (fstat(file->fd, &statbuf) != 0) {
        file->error = errno;
        return SQLITERK_IOERR;
    }
    *filesize = (size_t) statbuf.st_size;
    return SQLITERK_OK;
}

const char *sqliterkOSGetFilePath(sqliterk_file *file)
{
    return file->path;
}

void *sqliterkOSMalloc(size_t size)
{
    return calloc(size, sizeof(char));
}

void sqliterkOSFree(void *p)
{
    free(p);
}

static void
sqliterkDefaultLog(sqliterk_loglevel level, int result, const char *msg)
{
    fprintf(stderr, "[%s] %s\n", sqliterkGetResultCodeDescription(result), msg);
}

#define SQLITERK_CONFIG_MAXLOG 4096
static sqliterk_os s_os = {sqliterkDefaultLog};

int sqliterkOSLog(sqliterk_loglevel loglevel,
                  int result,
                  const char *format,
                  ...)
{
    char buf[SQLITERK_CONFIG_MAXLOG];

    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
    if (s_os.xLog) {
        s_os.xLog(loglevel, result, buf);
    }
    return result;
}

int sqliterkOSRegister(sqliterk_os os)
{
    s_os = os;
    return SQLITERK_OK;
}
