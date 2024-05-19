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

#ifdef __ANDROID__

#include "MMICUTokenizer.hpp"
#include "AndroidUnicodeDefine.h"
#include "SQLite.h"
#include <dirent.h>
#include <dlfcn.h>
#include <mutex>

WCDB_EXTERN_C_BEGIN

typedef struct icu_compat_t {
#define ICU_UC_FUNC(retType, funcName, argList)                                \
    retType(U_EXPORT2 *funcName##_) argList;
#define ICU_I18N_FUNC(retType, funcName, argList)                              \
    retType(U_EXPORT2 *funcName##_) argList;

#include "AndroidUnicodeFunctions.h"

#undef ICU_UC_FUNC
#undef ICU_I18N_FUNC
} ICUInterface;

static ICUInterface g_icuInterface = { nullptr };
static void *g_libicuuc_so = nullptr;
static void *g_libicui18n_so = nullptr;

#ifndef ICU_IMPL
#define ICU_DEFINE_SYMBOL(symbol) (g_icuInterface.symbol##_)

#include "AndroidUnicodeFunctions.h"

#undef ICU_UC_FUNC
#undef ICU_I18N_FUNC
#endif

WCDB_EXTERN_C_END

#define ROUND4(n) (((n) + 3) & ~3)
#define MINIMAL_OUT_BUFFER_LENGTH 512

namespace WCDB {

MMICUTokenizer::MMICUTokenizer(const char *const *azArg, int nArg, void *pCtx)
: AbstractFTSTokenizer(azArg, nArg, pCtx)
, m_iter(nullptr)
, m_originText(nullptr)
, m_originTextLength(0)
, m_inBuffer(nullptr)
, m_inLength(0)
, m_inOffset(nullptr)
, m_outBuffer(nullptr)
, m_outLength(0)
, m_tokenCount(0)
, m_ideoStart(0)
, m_ideoEnd(0)
, m_ideoState(0)
{
    if (nArg > 0) {
        strncpy(m_locale, azArg[0], 15);
        m_locale[15] = 0;
    } else {
        m_locale[0] = 0;
    }
}

MMICUTokenizer::~MMICUTokenizer()
{
    clearMemory();
}

void MMICUTokenizer::loadInput(const char *pText, int nText, int flags)
{
    if (nText < 0) {
        nText = (int) strlen(pText);
    }
    m_originText = pText, m_originTextLength = nText;

    clearMemory();

    m_tokenCount = 0;
    m_ideoStart = -1;
    m_ideoEnd = -1;
    m_ideoState = -1;
}

int MMICUTokenizer::nextToken(
const char **ppToken, int *nToken, int *iStart, int *iEnd, int *tflags, int *iPosition)
{
    if (!loadICUFunctionsFromSystemLibrary()) {
        return FTSError::Error();
    }
    int ret = initBRKIfNeeded();
    if (!FTSError::isOK(ret)) {
        return ret;
    }

    int32_t start, end;
    int32_t token_type;

    // process pending ideographic token.
    if (findSplitIDEOToken(&start, &end)) {
        return outputToken(start, end, ppToken, nToken, iStart, iEnd, iPosition);
    }

    start = ubrk_current(m_iter);

    // find first non-NONE token.
    for (;;) {
        end = ubrk_next(m_iter);
        if (end == UBRK_DONE) {
            // sqlite3_mm_clear_error();
            return FTSError::Done();
        }

        token_type = ubrk_getRuleStatus(m_iter);
        if (token_type >= UBRK_WORD_NONE && token_type < UBRK_WORD_NONE_LIMIT) {
            // look at the first character, if it's a space or ZWSP, ignore this token.
            // also ignore '*' because sqlite parser uses it as prefix operator.
            UChar32 c = m_inBuffer[start];
            if (c == '*' || c == 0x200b || u_isspace(c)) {
                start = end;
                continue;
            }
        }

        break;
    }

    // for non-IDEO tokens, just return.
    if (token_type < UBRK_WORD_IDEO || token_type >= UBRK_WORD_IDEO_LIMIT) {
        return outputToken(start, end, ppToken, nToken, iStart, iEnd, iPosition);
    }

    // for IDEO tokens, find all suffix ideo tokens.
    for (;;) {
        int32_t e = ubrk_next(m_iter);
        if (e == UBRK_DONE) break;
        token_type = ubrk_getRuleStatus(m_iter);
        if (token_type < UBRK_WORD_IDEO || token_type >= UBRK_WORD_IDEO_LIMIT)
            break;
        end = e;
    }

    ubrk_isBoundary(m_iter, end);

    m_ideoStart = start;
    m_ideoEnd = end;
    m_ideoState = 0;
    if (findSplitIDEOToken(&start, &end)) {
        return outputToken(start, end, ppToken, nToken, iStart, iEnd, iPosition);
    }
    // sqlite3_mm_set_last_error("IDEO token found but can't output token.");
    return FTSError::Error();
}

void MMICUTokenizer::clearMemory()
{
    if (m_inBuffer != nullptr) {
        sqlite3_free(m_inBuffer);
        m_inBuffer = nullptr;
        m_inOffset = nullptr;
        m_inLength = 0;
    }
    if (m_outBuffer != nullptr) {
        sqlite3_free(m_outBuffer);
        m_outBuffer = nullptr;
        m_outLength = 0;
    }

    if (m_iter) {
        ubrk_close(m_iter);
        m_iter = nullptr;
    }
}

int MMICUTokenizer::initBRKIfNeeded()
{
    if (m_originTextLength == 0 || m_outBuffer != nullptr) {
        return FTSError::OK();
    }

    int dst_len = ROUND4(m_originTextLength + 1);
    m_inBuffer = (UChar *) sqlite3_malloc((int) (dst_len * sizeof(UChar) // m_inBuffer
                                                 + (dst_len + 1) * sizeof(int))); // m_inOffset
    if (!m_inBuffer) {
        return FTSError::NoMem();
    }
    m_inOffset = (int *) (m_inBuffer + dst_len);

    UErrorCode status = U_ZERO_ERROR;
    UChar32 c;
    int i_input = 0;
    int i_output = 0;
    m_inOffset[i_output] = i_input;

    for (;;) {
        if (i_input >= m_originTextLength) {
            break;
        }

        U8_NEXT(m_originText, i_input, m_originTextLength, c);
        if (!c) {
            break;
        }
        if (c < 0) {
            c = ' ';
        }
        int is_error = 0;
        U16_APPEND(m_inBuffer, i_output, dst_len, c, is_error);
        if (is_error) {
            // sqlite3_mm_set_last_error("Writing UTF-16 character failed. Code point: 0x%x", c);
            return FTSError::Error();
        }
        m_inOffset[i_output] = i_input;
    }

    m_iter = ubrk_open(UBRK_WORD, m_locale, m_inBuffer, i_output, &status);
    if (U_FAILURE(status)) {
        // sqlite3_mm_set_last_error("Open UBreakIterator failed. ICU error code: %d", status);
        return FTSError::Error();
    }
    m_inLength = i_output;

    ubrk_first(m_iter);
    return FTSError::OK();
}

bool MMICUTokenizer::findSplitIDEOToken(int32_t *start, int32_t *end)
{
    if (m_ideoState < 0) {
        return false;
    }

    if (m_ideoStart == m_ideoEnd) {
        m_ideoState = -1;
        return false;
    }

    int32_t s, e;
    UChar32 c;

    // check UTF-16 surrogates, output 2 UChars if it's a lead surrogates, otherwise 1.
    s = m_ideoStart;
    e = s + 1;
    c = m_inBuffer[s];
    if (U16_IS_LEAD(c) && m_ideoEnd - s >= 2) {
        e++;
    }

    *start = s;
    *end = e;
    m_ideoStart = e;
    return true;
}

int MMICUTokenizer::outputToken(
int32_t start, int32_t end, const char **ppToken, int *nToken, int *iStart, int *iEnd, int *iPosition)
{
    UChar buf1[256];
    UChar buf2[256];
    UErrorCode status = U_ZERO_ERROR;
    int32_t result;
    int32_t length;

    length = end - start;
    if (length > 256) {
        length = 256;
    }
    result = unorm_normalize(
    m_inBuffer + start, length, UNORM_NFKD, 0, buf1, sizeof(buf1) / sizeof(UChar), &status);
    // currently, only try fixed length buffer, failed if overflowed.
    if (U_FAILURE(status) || result > sizeof(buf1) / sizeof(UChar)) {
        // char *seq = generate_token_printable_code(m_inBuffer + start, length);
        // sqlite3_mm_set_last_error("Normalize token failed. ICU status: %d, input: %s", status, seq);
        // free(seq);
        return FTSError::Error();
    }

    length = result;
    result = u_strFoldCase(
    buf2, sizeof(buf2) / sizeof(UChar), buf1, length, U_FOLD_CASE_DEFAULT, &status);
    // currently, only try fixed length buffer, failed if overflowed.
    if (U_FAILURE(status) || result > sizeof(buf2) / sizeof(UChar)) {
        // char *seq = generate_token_printable_code(buf1, length);
        // sqlite3_mm_set_last_error("FoldCase token failed. ICU status: %d, input: %s", status, seq);
        // free(seq);
        return FTSError::Error();
    }

    if (m_outBuffer == nullptr) {
        m_outBuffer = (char *) sqlite3_malloc(MINIMAL_OUT_BUFFER_LENGTH * sizeof(char));
        if (!m_outBuffer) return FTSError::NoMem();
        m_outLength = MINIMAL_OUT_BUFFER_LENGTH;
    }

    length = result;
    u_strToUTF8(m_outBuffer, m_outLength, &result, buf2, length, &status);
    if (result > m_outLength) {
        char *b = (char *) sqlite3_realloc(m_outBuffer, (int) (result * sizeof(char)));
        if (!b) return FTSError::NoMem();

        m_outBuffer = b;
        m_outLength = result;

        status = U_ZERO_ERROR;
        u_strToUTF8(m_outBuffer, m_outLength, &result, buf2, length, &status);
    }
    if (U_FAILURE(status) || result > m_outLength) {
        // char *seq = generate_token_printable_code(buf2, length);
        // sqlite3_mm_set_last_error("Transform token to UTF-8 failed. ICU status: %d, input: %s", status, seq);
        // free(seq);
        return FTSError::Error();
    }

    *ppToken = m_outBuffer;
    *nToken = result;
    *iStart = m_inOffset[start];
    *iEnd = m_inOffset[end];
    m_tokenCount++;
    if (iPosition != nullptr) {
        *iPosition = m_tokenCount;
    }
    return FTSError::OK();
}

#pragma mark - Load ICU
bool MMICUTokenizer::loadICUFunctionsFromSystemLibrary()
{
    static bool g_icuLoaded = false;
    static std::once_flag g_flag;
    std::call_once(g_flag, [&]() {
        char suffix[16];
        g_libicuuc_so = dlopen("libicuuc.so", RTLD_LAZY);
        g_libicui18n_so = dlopen("libicui18n.so", RTLD_LAZY);
        if (g_libicuuc_so != nullptr && g_libicui18n_so != nullptr
            && findICUSuffix(g_libicuuc_so, suffix, sizeof(suffix))) {
            loadFunctions(suffix);
            g_icuLoaded = true;
        } else {
            if (g_libicuuc_so) {
                dlclose(g_libicuuc_so);
            }
            if (g_libicui18n_so) {
                dlclose(g_libicui18n_so);
            }
        }
    });
    return g_icuLoaded;
}

void *MMICUTokenizer::loadFunction(void *libHandle, const char *funcName, const char *suffix)
{
    char buf[128];
    size_t len = strlcpy(buf, funcName, sizeof(buf));
    strlcpy(buf + len, suffix, sizeof(buf) - len);

    return dlsym(libHandle, buf);
}

void MMICUTokenizer::loadFunctions(const char *suffix)
{
#undef ICU_DEFINE_SYMBOL
#define ICU_UC_FUNC(retType, funcName, argList)                                \
    (g_icuInterface.funcName##_)                                               \
    = (retType(*) argList) loadFunction(g_libicuuc_so, #funcName, suffix);
#define ICU_I18N_FUNC(retType, funcName, argList)                              \
    (g_icuInterface.funcName##_)                                               \
    = (retType(*) argList) loadFunction(g_libicui18n_so, #funcName, suffix);

#include "AndroidUnicodeFunctions.h"

#undef ICU_UC_FUNC
#undef ICU_I18N_FUNC
}

const char *MMICUTokenizer::findICUSuffix(void *libHandle, char *buf, size_t bufLen)
{
    DIR *icuDir = opendir("/system/usr/icu");
    if (!icuDir) return nullptr;

    struct dirent *dp;
    while ((dp = readdir(icuDir)) != nullptr) {
        if (dp->d_type == DT_REG) {
            // Find /system/usr/icu/icudt??l.dat
            const char *name = dp->d_name;
            if (strlen(name) == 12 && !strncmp("icudt", name, 5)
                && name[5] >= '0' && name[5] <= '9' && name[6] >= '0'
                && name[6] <= '9' && !strncmp("l.dat", &name[7], 6)) {
                snprintf(buf, bufLen, "_%c%c", name[5], name[6]);

                // Try loading with suffix.
                if (loadFunction(libHandle, "u_getVersion", buf)) {
                    closedir(icuDir);
                    return buf;
                }
            }
        }
    }
    closedir(icuDir);

    // Failed, try plain names without suffix.
    if (loadFunction(libHandle, "u_getVersion", "")) {
        buf[0] = 0;
        return buf;
    }

    return nullptr;
}

} // namespace WCDB

#endif