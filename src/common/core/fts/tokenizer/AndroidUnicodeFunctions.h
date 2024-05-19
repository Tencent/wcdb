// Created by chenqiuwen on 2024/5/19.
//

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

#ifndef ICU_UC_FUNC
#define ICU_UC_FUNC(ret_type, func_name, arg_list)
#endif

#ifndef ICU_I18N_FUNC
#define ICU_I18N_FUNC(ret_type, func_name, arg_list)
#endif

#ifdef ICU_DEFINE_SYMBOL

/* uchar functions */
#define u_isspace ICU_DEFINE_SYMBOL(u_isspace)

/* ustring functions */
#define u_strFoldCase ICU_DEFINE_SYMBOL(u_strFoldCase)
#define u_strToUTF8 ICU_DEFINE_SYMBOL(u_strToUTF8)

/* utf8 functions */
#define utf8_nextCharSafeBody ICU_DEFINE_SYMBOL(utf8_nextCharSafeBody)

/* ubrk functions */
#define ubrk_open ICU_DEFINE_SYMBOL(ubrk_open)
#define ubrk_close ICU_DEFINE_SYMBOL(ubrk_close)
#define ubrk_current ICU_DEFINE_SYMBOL(ubrk_current)
#define ubrk_next ICU_DEFINE_SYMBOL(ubrk_next)
#define ubrk_first ICU_DEFINE_SYMBOL(ubrk_first)
#define ubrk_isBoundary ICU_DEFINE_SYMBOL(ubrk_isBoundary)
#define ubrk_getRuleStatus ICU_DEFINE_SYMBOL(ubrk_getRuleStatus)

/* unorm functions */
#define unorm_normalize ICU_DEFINE_SYMBOL(unorm_normalize)

#else

/* uchar functions */
ICU_UC_FUNC(UBool, u_isspace, (UChar32 c))

/* ustring functions */
ICU_UC_FUNC(int32_t,
            u_strFoldCase,
            (UChar * dest, int32_t destCapacity, const UChar *src, int32_t srcLength, uint32_t options, UErrorCode *pErrorCode))
ICU_UC_FUNC(char *,
            u_strToUTF8,
            (char *dest, int32_t destCapacity, int32_t *pDestLength, const UChar *src, int32_t srcLength, UErrorCode *pErrorCode))

/* utf8 functions */
ICU_UC_FUNC(UChar32,
            utf8_nextCharSafeBody,
            (const uint8_t *s, int32_t *pi, int32_t length, UChar32 c, UBool strict))

/* ubrk functions */
ICU_UC_FUNC(UBreakIterator *,
            ubrk_open,
            (UBreakIteratorType type, const char *locale, const UChar *text, int32_t textLength, UErrorCode *status))
ICU_UC_FUNC(void, ubrk_close, (UBreakIterator * bi))
ICU_UC_FUNC(int32_t, ubrk_current, (const UBreakIterator *bi))
ICU_UC_FUNC(int32_t, ubrk_next, (UBreakIterator * bi))
ICU_UC_FUNC(int32_t, ubrk_first, (UBreakIterator * bi))
ICU_UC_FUNC(UBool, ubrk_isBoundary, (UBreakIterator * bi, int32_t offset))
ICU_UC_FUNC(int32_t, ubrk_getRuleStatus, (UBreakIterator * bi))

/* unorm functions */
ICU_UC_FUNC(int32_t,
            unorm_normalize,
            (const UChar *source,
             int32_t sourceLength,
             UNormalizationMode mode,
             int32_t options,
             UChar *result,
             int32_t resultLength,
             UErrorCode *status))
#endif
