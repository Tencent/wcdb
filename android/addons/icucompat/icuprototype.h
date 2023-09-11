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

#ifndef ICUCOMPAT_UC_FUNC
#define ICUCOMPAT_UC_FUNC(ret_type, func_name, arg_list)
#endif

#ifndef ICUCOMPAT_I18N_FUNC
#define ICUCOMPAT_I18N_FUNC(ret_type, func_name, arg_list)
#endif


#ifdef ICUCOMPAT_DEFINE_SYMBOL

/* uversion functions */
#define u_getVersion ICUCOMPAT_DEFINE_SYMBOL(u_getVersion)

/* uchar functions */
#define u_isspace ICUCOMPAT_DEFINE_SYMBOL(u_isspace)

/* ustring functions */
#define u_strFoldCase ICUCOMPAT_DEFINE_SYMBOL(u_strFoldCase)
#define u_strToUTF8 ICUCOMPAT_DEFINE_SYMBOL(u_strToUTF8)
#define u_strtok_r ICUCOMPAT_DEFINE_SYMBOL(u_strtok_r)

/* utf8 functions */
#define utf8_nextCharSafeBody ICUCOMPAT_DEFINE_SYMBOL(utf8_nextCharSafeBody)

/* ubrk functions */
#define ubrk_open  ICUCOMPAT_DEFINE_SYMBOL(ubrk_open)
#define ubrk_close ICUCOMPAT_DEFINE_SYMBOL(ubrk_close)
#define ubrk_setText ICUCOMPAT_DEFINE_SYMBOL(ubrk_setText)
#define ubrk_current ICUCOMPAT_DEFINE_SYMBOL(ubrk_current)
#define ubrk_next ICUCOMPAT_DEFINE_SYMBOL(ubrk_next)
#define ubrk_previous ICUCOMPAT_DEFINE_SYMBOL(ubrk_previous)
#define ubrk_first ICUCOMPAT_DEFINE_SYMBOL(ubrk_first)
#define ubrk_isBoundary ICUCOMPAT_DEFINE_SYMBOL(ubrk_isBoundary)
#define ubrk_getRuleStatus ICUCOMPAT_DEFINE_SYMBOL(ubrk_getRuleStatus)

/* unorm functions */
#define unorm_normalize ICUCOMPAT_DEFINE_SYMBOL(unorm_normalize)

/* ucnv functions */
#define ucnv_fromUChars ICUCOMPAT_DEFINE_SYMBOL(ucnv_fromUChars)
#define ucnv_toUChars ICUCOMPAT_DEFINE_SYMBOL(ucnv_toUChars)
#define ucnv_openCCSID ICUCOMPAT_DEFINE_SYMBOL(ucnv_openCCSID)
#define ucnv_close ICUCOMPAT_DEFINE_SYMBOL(ucnv_close)

/* uiter functions */
#define uiter_setUTF8 ICUCOMPAT_DEFINE_SYMBOL(uiter_setUTF8)

/* ucol functions */
#define ucol_strcoll ICUCOMPAT_DEFINE_SYMBOL(ucol_strcoll)
#define ucol_strcollIter ICUCOMPAT_DEFINE_SYMBOL(ucol_strcollIter)
#define ucol_getSortKey ICUCOMPAT_DEFINE_SYMBOL(ucol_getSortKey)
#define ucol_open ICUCOMPAT_DEFINE_SYMBOL(ucol_open)
#define ucol_setAttribute ICUCOMPAT_DEFINE_SYMBOL(ucol_setAttribute)
#define ucol_getShortDefinitionString ICUCOMPAT_DEFINE_SYMBOL(ucol_getShortDefinitionString)

#else

/* uversion functions */
ICUCOMPAT_UC_FUNC(void, u_getVersion, (UVersionInfo versionArray))

/* uchar functions */
ICUCOMPAT_UC_FUNC(UBool, u_isspace, (UChar32 c))

/* ustring functions */
ICUCOMPAT_UC_FUNC(int32_t, u_strFoldCase, (UChar *dest, int32_t destCapacity, const UChar *src, int32_t srcLength, uint32_t options, UErrorCode *pErrorCode))
ICUCOMPAT_UC_FUNC(char *, u_strToUTF8, (char *dest, int32_t destCapacity, int32_t *pDestLength, const UChar *src, int32_t srcLength, UErrorCode *pErrorCode))
ICUCOMPAT_UC_FUNC(UChar *, u_strtok_r, (UChar *src, const UChar *delim, UChar **saveState))

/* utf8 functions */
ICUCOMPAT_UC_FUNC(UChar32, utf8_nextCharSafeBody, (const uint8_t *s, int32_t *pi, int32_t length, UChar32 c, UBool strict))

/* ubrk functions */
ICUCOMPAT_UC_FUNC(UBreakIterator *, ubrk_open, (UBreakIteratorType type, const char *locale, const UChar *text, int32_t textLength, UErrorCode *status))
ICUCOMPAT_UC_FUNC(void, ubrk_close, (UBreakIterator *bi))
ICUCOMPAT_UC_FUNC(void, ubrk_setText, (UBreakIterator* bi, const UChar* text, int32_t textLength, UErrorCode *status))
ICUCOMPAT_UC_FUNC(int32_t, ubrk_current, (const UBreakIterator *bi))
ICUCOMPAT_UC_FUNC(int32_t, ubrk_next, (UBreakIterator *bi))
ICUCOMPAT_UC_FUNC(int32_t, ubrk_previous, (UBreakIterator *bi))
ICUCOMPAT_UC_FUNC(int32_t, ubrk_first, (UBreakIterator *bi))
ICUCOMPAT_UC_FUNC(UBool, ubrk_isBoundary, (UBreakIterator *bi, int32_t offset))
ICUCOMPAT_UC_FUNC(int32_t, ubrk_getRuleStatus, (UBreakIterator *bi))

/* unorm functions */
ICUCOMPAT_UC_FUNC(int32_t, unorm_normalize, (const UChar *source, int32_t sourceLength, UNormalizationMode mode, int32_t options, UChar *result, int32_t resultLength, UErrorCode *status))

/* ucnv functions */
ICUCOMPAT_UC_FUNC(int32_t, ucnv_fromUChars, (UConverter *cnv, char *dest, int32_t destCapacity, const UChar *src, int32_t srcLength, UErrorCode *pErrorCode))
ICUCOMPAT_UC_FUNC(int32_t, ucnv_toUChars, (UConverter *cnv, UChar *dest, int32_t destCapacity, const char *src, int32_t srcLength, UErrorCode *pErrorCode))
ICUCOMPAT_UC_FUNC(UConverter *, ucnv_openCCSID, (int32_t codepage, UConverterPlatform platform, UErrorCode *err))
ICUCOMPAT_UC_FUNC(void, ucnv_close, (UConverter *converter))

/* uiter functions */
ICUCOMPAT_UC_FUNC(void, uiter_setUTF8, (UCharIterator *iter, const char *s, int32_t length))

/* ucol functions */
ICUCOMPAT_I18N_FUNC(UCollationResult, ucol_strcoll, (const UCollator *coll, const UChar *source, int32_t sourceLength, const UChar *target, int32_t targetLength))
ICUCOMPAT_I18N_FUNC(UCollationResult, ucol_strcollIter, (const UCollator *coll, UCharIterator *sIter, UCharIterator *tIter, UErrorCode *status))
ICUCOMPAT_I18N_FUNC(int32_t, ucol_getSortKey, (const UCollator *coll, const UChar *source, int32_t sourceLength, uint8_t *result, int32_t resultLength))
ICUCOMPAT_I18N_FUNC(UCollator*, ucol_open, (const char *loc, UErrorCode *status))
ICUCOMPAT_I18N_FUNC(void, ucol_setAttribute, (UCollator *coll, UColAttribute attr, UColAttributeValue value, UErrorCode *status))
ICUCOMPAT_I18N_FUNC(int32_t, ucol_getShortDefinitionString, (const UCollator *coll, const char *locale, char *buffer, int32_t capacity, UErrorCode *status))

#endif