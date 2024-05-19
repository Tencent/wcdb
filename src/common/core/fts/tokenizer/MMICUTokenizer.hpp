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

#pragma once

#include "TokenizerModuleTemplate.hpp"

/**
 * The following definition is copied from the Unicode library in Android system
 */
typedef struct UBreakIterator UBreakIterator;
#if defined(UCHAR_TYPE)
typedef UCHAR_TYPE UChar;
/* Not #elif U_HAVE_CHAR16_T -- because that is type-incompatible with pre-C++11 callers
    typedef char16_t UChar;  */
#elif U_SIZEOF_WCHAR_T == 2
typedef wchar_t UChar;
#elif defined(__CHAR16_TYPE__)
typedef __CHAR16_TYPE__ UChar;
#else
typedef uint16_t UChar;
#endif

namespace WCDB {

class WCDB_API MMICUTokenizer final : public AbstractFTSTokenizer {
public:
    MMICUTokenizer(const char *const *azArg, int nArg, void *pCtx);
    ~MMICUTokenizer();
    void loadInput(const char *pText, int nText, int flags) override;
    int nextToken(const char **ppToken, int *nToken, int *iStart, int *iEnd, int *tflags, int *iPosition) override;

private:
    void clearMemory();
    int initBRKIfNeeded();
    bool findSplitIDEOToken(int32_t *start, int32_t *end);
    int outputToken(int32_t start,
                    int32_t end,
                    const char **ppToken,
                    int *nToken,
                    int *iStart,
                    int *iEnd,
                    int *iPosition);

    char m_locale[16]{};

    UBreakIterator *m_iter; // UBreakIterator for the text.

    const char *m_originText; // origin input text, in UTF-8.
    int m_originTextLength;   // length of m_originText

    UChar *m_inBuffer; // whole input text buffer, in UTF-16.
    int m_inLength;    // input text length.

    int *m_inOffset; // offset of each utf8 token

    char *m_outBuffer; // output token buffer, int UTF-8,
    int m_outLength;   // output token buffer length.

    int m_tokenCount;
    int32_t m_ideoStart; // ideographic unary/binary tokenizing cursor.
    int32_t m_ideoEnd;   // ideographic unary/binary tokenizing end point.
    int m_ideoState;     // 0 for unary output, -1 for invalid status.

#pragma mark - Load ICU
private:
    static bool loadICUFunctionsFromSystemLibrary();
    static void *loadFunction(void *libHandle, const char *funcName, const char *suffix);
    static void loadFunctions(const char *suffix);
    static const char *findICUSuffix(void *libHandle, char *buf, size_t bufLen);
};

} //namespace WCDB
