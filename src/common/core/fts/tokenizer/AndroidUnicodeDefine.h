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

#pragma once

#include "Macro.h"
#include <cstdlib>
#include <cstring>

/**
 * The following definition is copied from the Unicode library in Android system
 */

WCDB_EXTERN_C_BEGIN

typedef enum UErrorCode {
    U_ZERO_ERROR = 0, /**< No error, no warning. */
} UErrorCode;

typedef int32_t UChar32;

typedef int8_t UBool;

/**
 * \def U_CDECL_END
 * This is used to end a declaration of a library private ICU C API
 * @stable ICU 2.4
 */

#ifdef __cplusplus
#define U_CFUNC extern "C"
#define U_CDECL_BEGIN extern "C" {
#define U_CDECL_END }
#else
#define U_CFUNC extern
#define U_CDECL_BEGIN
#define U_CDECL_END
#endif

#ifdef __has_declspec_attribute
#define UPRV_HAS_DECLSPEC_ATTRIBUTE(x) __has_declspec_attribute(x)
#else
#define UPRV_HAS_DECLSPEC_ATTRIBUTE(x) 0
#endif

#ifdef U_EXPORT
/* Use the predefined value. */
#elif defined(U_STATIC_IMPLEMENTATION)
#define U_EXPORT
#elif defined(_MSC_VER)                                                        \
|| (UPRV_HAS_DECLSPEC_ATTRIBUTE(__dllexport__) && UPRV_HAS_DECLSPEC_ATTRIBUTE(__dllimport__))
#define U_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define U_EXPORT __attribute__((visibility("default")))
#elif (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x550)                           \
|| (defined(__SUNPRO_C) && __SUNPRO_C >= 0x550)
#define U_EXPORT __global
/*#elif defined(__HP_aCC) || defined(__HP_cc)
#   define U_EXPORT __declspec(dllexport)*/
#else
#define U_EXPORT
#endif

/* U_CALLCONV is related to U_EXPORT2 */
#ifdef U_EXPORT2
/* Use the predefined value. */
#elif defined(_MSC_VER)
#define U_EXPORT2 __cdecl
#else
#define U_EXPORT2
#endif

/** This is used to declare a function as a public ICU C API @stable ICU 2.0*/
#define U_CAPI U_CFUNC U_EXPORT

/** This is used to declare a function as a stable public ICU C API*/
#define U_STABLE U_CAPI

/** The possible types of text boundaries.  @stable ICU 2.0 */
typedef enum UBreakIteratorType {
    /** Character breaks  @stable ICU 2.0 */
    UBRK_CHARACTER = 0,
    /** Word breaks @stable ICU 2.0 */
    UBRK_WORD = 1,
    /** Line breaks @stable ICU 2.0 */
    UBRK_LINE = 2,
    /** Sentence breaks @stable ICU 2.0 */
    UBRK_SENTENCE = 3,

#ifndef U_HIDE_DEPRECATED_API
    /**
     * Title Case breaks
     * The iterator created using this type locates title boundaries as described for
     * Unicode 3.2 only. For Unicode 4.0 and above title boundary iteration,
     * please use Word Boundary iterator.
     *
     * @deprecated ICU 2.8 Use the word break iterator for titlecasing for Unicode 4 and later.
     */
    UBRK_TITLE = 4,
#endif /* U_HIDE_DEPRECATED_API */
    UBRK_COUNT = 5
} UBreakIteratorType;

/**
 * Set the iterator position to the index of the first character in the text being scanned.
 * This is not always the same as index 0 of the text.
 * @param bi The break iterator to use.
 * @return The character index of the first character in the text being scanned.
 * @see ubrk_last
 * @stable ICU 2.0
 */
U_STABLE int32_t U_EXPORT2 ubrk_first(UBreakIterator *bi);

#define UBRK_DONE ((int32_t) -1)

typedef enum UWordBreak {
    /** Tag value for "words" that do not fit into any of other categories.
     *  Includes spaces and most punctuation. */
    UBRK_WORD_NONE = 0,
    /** Upper bound for tags for uncategorized words. */
    UBRK_WORD_NONE_LIMIT = 100,
    /** Tag value for words that appear to be numbers, lower limit.    */
    UBRK_WORD_NUMBER = 100,
    /** Tag value for words that appear to be numbers, upper limit.    */
    UBRK_WORD_NUMBER_LIMIT = 200,
    /** Tag value for words that contain letters, excluding
     *  hiragana, katakana or ideographic characters, lower limit.    */
    UBRK_WORD_LETTER = 200,
    /** Tag value for words containing letters, upper limit  */
    UBRK_WORD_LETTER_LIMIT = 300,
    /** Tag value for words containing kana characters, lower limit */
    UBRK_WORD_KANA = 300,
    /** Tag value for words containing kana characters, upper limit */
    UBRK_WORD_KANA_LIMIT = 400,
    /** Tag value for words containing ideographic characters, lower limit */
    UBRK_WORD_IDEO = 400,
    /** Tag value for words containing ideographic characters, upper limit */
    UBRK_WORD_IDEO_LIMIT = 500
} UWordBreak;

/**
 * Constants for normalization modes.
 * @stable ICU 2.0
 */
typedef enum {
    /** No decomposition/composition. @stable ICU 2.0 */
    UNORM_NONE = 1,
    /** Canonical decomposition. @stable ICU 2.0 */
    UNORM_NFD = 2,
    /** Compatibility decomposition. @stable ICU 2.0 */
    UNORM_NFKD = 3,
    /** Canonical decomposition followed by canonical composition. @stable ICU 2.0 */
    UNORM_NFC = 4,
    /** Default normalization. @stable ICU 2.0 */
    UNORM_DEFAULT = UNORM_NFC,
    /** Compatibility decomposition followed by canonical composition. @stable ICU 2.0 */
    UNORM_NFKC = 5,
    /** "Fast C or D" form. @stable ICU 2.0 */
    UNORM_FCD = 6,

    /** One more than the highest normalization mode constant. @stable ICU 2.0 */
    UNORM_MODE_COUNT
} UNormalizationMode;

/**
 * Option value for case folding: Use default mappings defined in CaseFolding.txt.
 *
 * @stable ICU 2.0
 */
#define U_FOLD_CASE_DEFAULT 0

/**
 * Is this code unit (byte) a UTF-8 lead byte? (0xC2..0xF4)
 * @param c 8-bit code unit (byte)
 * @return true or false
 * @stable ICU 2.4
 */
#define U8_IS_LEAD(c) ((uint8_t) ((c) -0xc2) <= 0x32)
// 0x32=0xf4-0xc2

/**
 * This value is intended for sentinel values for APIs that
 * (take or) return single code points (UChar32).
 * It is outside of the Unicode code point range 0..0x10ffff.
 *
 * For example, a "done" or "error" value in a new API
 * could be indicated with U_SENTINEL.
 *
 * ICU APIs designed before ICU 2.4 usually define service-specific "done"
 * values, mostly 0xffff.
 * Those may need to be distinguished from
 * actual U+ffff text contents by calling functions like
 * CharacterIterator::hasNext() or UnicodeString::length().
 *
 * @return -1
 * @see UChar32
 * @stable ICU 2.4
 */
#define U_SENTINEL (-1)

/**
 * Get a code point from a string at a code point boundary offset,
 * and advance the offset to the next code point boundary.
 * (Post-incrementing forward iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The offset may point to the lead byte of a multi-byte sequence,
 * in which case the macro will read the whole sequence.
 * If the offset points to a trail byte or an illegal UTF-8 sequence, then
 * c is set to a negative value.
 *
 * @param s const uint8_t * string
 * @param i string offset, must be i<length
 * @param length string length
 * @param c output UChar32 variable, set to <0 in case of an error
 * @see U8_NEXT_UNSAFE
 * @stable ICU 2.4
 */
#define U8_NEXT(s, i, length, c)                                                                                 \
    {                                                                                                            \
        (c) = (uint8_t) (s)[(i)++];                                                                              \
        if ((c) >= 0x80) {                                                                                       \
            uint8_t __t1, __t2;                                                                                  \
            if (/* handle U+1000..U+CFFF inline */                                                               \
                (0xe0 < (c) && (c) <= 0xec) && (((i) + 1) < (length))                                            \
                && (__t1 = (uint8_t) ((s)[i] - 0x80)) <= 0x3f                                                    \
                && (__t2 = (uint8_t) ((s)[(i) + 1] - 0x80)) <= 0x3f) {                                           \
                /* no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (UChar) */ \
                (c) = (UChar) (((c) << 12) | (__t1 << 6) | __t2);                                                \
                (i) += 2;                                                                                        \
            } else if (/* handle U+0080..U+07FF inline */                                                        \
                       ((c) < 0xe0 && (c) >= 0xc2) && ((i) < (length))                                           \
                       && (__t1 = (uint8_t) ((s)[i] - 0x80)) <= 0x3f) {                                          \
                (c) = (UChar) ((((c) &0x1f) << 6) | __t1);                                                       \
                ++(i);                                                                                           \
            } else if (U8_IS_LEAD(c)) {                                                                          \
                /* function call for "complicated" and error cases */                                            \
                (c) = utf8_nextCharSafeBody(                                                                     \
                (const uint8_t *) s, &(i), (int32_t) (length), c, -1);                                           \
            } else {                                                                                             \
                (c) = U_SENTINEL;                                                                                \
            }                                                                                                    \
        }                                                                                                        \
    }

/**
 * \def UPRV_BLOCK_MACRO_BEGIN
 * Defined as the "do" keyword by default.
 * @internal
 */
#ifndef UPRV_BLOCK_MACRO_BEGIN
#define UPRV_BLOCK_MACRO_BEGIN do
#endif

/**
 * \def UPRV_BLOCK_MACRO_END
 * Defined as "while (false)" by default.
 * @internal
 */
#ifndef UPRV_BLOCK_MACRO_END
#define UPRV_BLOCK_MACRO_END while (false)
#endif

/**
 * Append a code point to a string, overwriting 1 or 2 code units.
 * The offset points to the current end of the string contents
 * and is advanced (post-increment).
 * "Safe" macro, checks for a valid code point.
 * If a surrogate pair is written, checks for sufficient space in the string.
 * If the code point is not valid or a trail surrogate does not fit,
 * then isError is set to true.
 *
 * @param s const UChar * string buffer
 * @param i string offset, must be i<capacity
 * @param capacity size of the string buffer
 * @param c code point to append
 * @param isError output UBool set to true if an error occurs, otherwise not modified
 * @see U16_APPEND_UNSAFE
 * @stable ICU 2.4
 */
#define U16_APPEND(s, i, capacity, c, isError)                                 \
    UPRV_BLOCK_MACRO_BEGIN                                                     \
    {                                                                          \
        if ((uint32_t) (c) <= 0xffff) {                                        \
            (s)[(i)++] = (uint16_t) (c);                                       \
        } else if ((uint32_t) (c) <= 0x10ffff && (i) + 1 < (capacity)) {       \
            (s)[(i)++] = (uint16_t) (((c) >> 10) + 0xd7c0);                    \
            (s)[(i)++] = (uint16_t) (((c) &0x3ff) | 0xdc00);                   \
        } else /* c>0x10ffff or not enough space */ {                          \
            (isError) = true;                                                  \
        }                                                                      \
    }                                                                          \
    UPRV_BLOCK_MACRO_END

/**
 * Is this code unit a lead surrogate (U+d800..U+dbff)?
 * @param c 16-bit code unit
 * @return true or false
 * @stable ICU 2.4
 */
#define U16_IS_LEAD(c) (((c) &0xfffffc00) == 0xd800)

/**
 * Does the error code indicate success?
 * @stable ICU 2.0
 */
#define U_SUCCESS(x) ((x) <= U_ZERO_ERROR)
/**
 * Does the error code indicate a failure?
 * @stable ICU 2.0
 */
#define U_FAILURE(x) ((x) > U_ZERO_ERROR)

WCDB_EXTERN_C_END
