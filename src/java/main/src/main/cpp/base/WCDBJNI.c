// Created by qiuwenchen on 2023/3/30.
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

#include "WCDBJNI.h"
#include "ObjectBridge.h"
#include "assert.h"
#include <string.h>

#define LIKELY(exp) (__builtin_expect((exp) != 0, true))
#define UNLIKELY(exp) (__builtin_expect((exp) != 0, false))

JavaVM* g_vm = NULL;

void WCDBJNIDestructContext(jobject config)
{
    WCDBJNITryGetEnvOr(return );
    (*env)->DeleteGlobalRef(env, config);
    WCDBJNITryDetach;
}

void WCDBJNIClassMethod(Base, releaseObject, long long cppObject)
{
    WCDBReleaseCPPObject((CPPObject*) cppObject);
}

jclass g_databaseClass = NULL;
jclass g_handleClass = NULL;
jclass g_exceptionClass = NULL;

void WCDBJNIInitJClasses(JNIEnv* env)
{
    g_databaseClass = (*env)->FindClass(env, "com/tencent/wcdb/core/Database");
    WCDBJNICreateGlobalRel(g_databaseClass);
    assert(g_databaseClass != NULL);

    g_handleClass = (*env)->FindClass(env, "com/tencent/wcdb/core/Handle");
    WCDBJNICreateGlobalRel(g_handleClass);
    assert(g_handleClass != NULL);

    g_exceptionClass = (*env)->FindClass(env, "com/tencent/wcdb/base/WCDBException");
    WCDBJNICreateGlobalRel(g_exceptionClass);
    assert(g_exceptionClass != NULL);
}

jclass WCDBJNIGetDatabaseClass()
{
    return g_databaseClass;
}

jclass WCDBJNIGetHandleClass()
{
    return g_handleClass;
}

jclass WCDBJNIGetExceptionClass()
{
    return g_exceptionClass;
}

static jsize utf16_to_utf8_length(const jchar* src, jsize src_len);
static void utf16_to_utf8(const jchar* src, jsize src_len, char* dst, jsize dst_len);
static jsize utf8_to_utf16_length(const char* u8str, jsize u8len);
static jchar* utf8_to_utf16(const char* u8str, jsize u8len, jchar* u16str, jsize u16len);

void WCDBJNIGetUTF8String(JNIEnv* env, jstring value, char** utf8String, const jchar** utf16String)
{
    if (UNLIKELY(value == NULL)) {
        *utf8String = NULL;
        return;
    }
    jsize utf16Length = (*env)->GetStringLength(env, value);
    if (UNLIKELY(utf16Length == 0)) {
        *utf8String = NULL;
        return;
    }
    *utf16String = (*env)->GetStringChars(env, value, 0);
    jsize utf8Length = utf16_to_utf8_length(*utf16String, utf16Length);
    char** preAllocSlot = WCDBPreAllocStringMemorySlot(1);
    if (UNLIKELY(preAllocSlot == NULL)) {
        *utf8String = NULL;
        return;
    }
    WCDBAllocStringMemory(preAllocSlot, (int) utf8Length);
    if (UNLIKELY(*preAllocSlot == NULL)) {
        *utf8String = NULL;
        return;
    }
    *utf8String = *preAllocSlot;
    utf16_to_utf8(*utf16String, utf16Length, *utf8String, utf8Length);
}

void WCDBJNIGetUTF8StringArray(JNIEnv* env, jobjectArray value, char*** stringArray, int* length)
{
    if (UNLIKELY(value == NULL)) {
        return;
    }
    int valueLength = (*env)->GetArrayLength(env, value);
    if (UNLIKELY(valueLength <= 0)) {
        return;
    }
    char** preAllocSlot = WCDBPreAllocStringMemorySlot(valueLength);
    if (UNLIKELY(preAllocSlot == NULL)) {
        return;
    }
    for (int i = 0; i < valueLength; i++) {
        jstring curString = (jstring) (*env)->GetObjectArrayElement(env, value, i);
        if (UNLIKELY(curString == NULL)) {
            continue;
        }
        jsize utf16Length = (*env)->GetStringLength(env, curString);
        const jchar* utf16String = (*env)->GetStringCritical(env, curString, 0);
        jsize utf8Length = utf16_to_utf8_length(utf16String, utf16Length);
        char** curSlot = preAllocSlot + i;
        WCDBAllocStringMemory(curSlot, utf8Length);
        if (UNLIKELY(*curSlot == NULL)) {
            (*env)->ReleaseStringCritical(env, curString, utf16String);
            (*env)->DeleteLocalRef(env, curString);
            WCDBClearAllocatedMemory(valueLength);
            return;
        }
        utf16_to_utf8(utf16String, utf16Length, *curSlot, utf8Length);
        (*env)->ReleaseStringCritical(env, curString, utf16String);
        (*env)->DeleteLocalRef(env, curString);
    }
    *length = valueLength;
    *stringArray = preAllocSlot;
}

jstring WCDBJNICreateJString(JNIEnv* env, const char* utf8String)
{
    if (utf8String == NULL) {
        return NULL;
    }
    jsize u8len = (jsize) strlen(utf8String);
    jsize utf16Length = utf8_to_utf16_length(utf8String, u8len);
    jchar* utf16Buffer = NULL;
    bool needFree = false;
    if (LIKELY(utf16Length < 1000)) {
        utf16Buffer = alloca((utf16Length + 1) * sizeof(jchar));
    } else {
        utf16Buffer = malloc((utf16Length + 1) * sizeof(jchar));
        needFree = true;
    }
    if (UNLIKELY(utf16Buffer == NULL)) {
        return NULL;
    }
    jchar* utf16End = utf8_to_utf16(utf8String, u8len, utf16Buffer, utf16Length + 1);
    jstring ret;
    if (LIKELY(utf16End > utf16Buffer)) {
        ret = (*env)->NewString(env, utf16Buffer, utf16End - utf16Buffer);
    } else {
        ret = (*env)->NewString(env, utf16Buffer, 0);
    }
    if (UNLIKELY(needFree)) {
        free(utf16Buffer);
    }
    return ret;
}

/*
 * The code below is copied from:
 * https://cs.android.com/android/platform/superproject/main/+/main:system/core/libutils/Unicode.cpp;l=1?q=Unicode.cpp&ss=android%2Fplatform%2Fsuperproject%2Fmain
 */

// is_any_surrogate() returns true if w is either a high or low surrogate
static bool is_any_surrogate(jchar w)
{
    return (w & 0xf800) == 0xd800;
}

// is_surrogate_pair() returns true if w1 and w2 form a valid surrogate pair
static bool is_surrogate_pair(jchar w1, jchar w2)
{
    return ((w1 & 0xfc00) == 0xd800) && ((w2 & 0xfc00) == 0xdc00);
}

static jsize utf16_to_utf8_length(const jchar* src, jsize src_len)
{
    if (src == NULL || src_len == 0) return 0;

    const jchar* const end = src + src_len;
    const jchar* in = src;
    jsize utf8_len = 0;

    while (in < end) {
        jchar w = *in++;
        if (LIKELY(w < 0x0080)) {
            utf8_len += 1;
            continue;
        }
        if (LIKELY(w < 0x0800)) {
            utf8_len += 2;
            continue;
        }
        if (LIKELY(!is_any_surrogate(w))) {
            utf8_len += 3;
            continue;
        }
        if (in < end && is_surrogate_pair(w, *in)) {
            utf8_len += 4;
            in++;
            continue;
        }
        /* skip if at the end of the string or invalid surrogate pair */
    }
    return utf8_len;
}

static void utf16_to_utf8(const jchar* src, jsize src_len, char* dst, jsize dst_len)
{
    if (src == NULL || src_len == 0 || dst == NULL) {
        return;
    }

    const jchar* in = src;
    const jchar* const in_end = src + src_len;
    char* out = dst;
    const char* const out_end = dst + dst_len;
    jchar w2;

    while (in < in_end) {
        jchar w = *in++;
        if (LIKELY(w < 0x0080)) {
            if (out + 1 > out_end) abort();
            *out++ = (char) (w & 0xff);
            continue;
        }
        if (LIKELY(w < 0x0800)) {
            if (out + 2 > out_end) abort();
            *out++ = (char) (0xc0 | ((w >> 6) & 0x1f));
            *out++ = (char) (0x80 | ((w >> 0) & 0x3f));
            continue;
        }
        if (LIKELY(!is_any_surrogate(w))) {
            if (out + 3 > out_end) abort();
            *out++ = (char) (0xe0 | ((w >> 12) & 0xf));
            *out++ = (char) (0x80 | ((w >> 6) & 0x3f));
            *out++ = (char) (0x80 | ((w >> 0) & 0x3f));
            continue;
        }
        /* surrogate pair */
        if (in < in_end && (w2 = *in, is_surrogate_pair(w, w2))) {
            if (out + 4 > out_end) abort();
            jint dw = (jint) (0x10000 + ((w - 0xd800) << 10) + (w2 - 0xdc00));
            *out++ = (char) (0xf0 | ((dw >> 18) & 0x07));
            *out++ = (char) (0x80 | ((dw >> 12) & 0x3f));
            *out++ = (char) (0x80 | ((dw >> 6) & 0x3f));
            *out++ = (char) (0x80 | ((dw >> 0) & 0x3f));
            in++;
        }
        /* We reach here in two cases:
         *  1) (in == in_end), which means end of the input string
         *  2) (w2 & 0xfc00) != 0xdc00, which means invalid surrogate pair
         * In either case, we intentionally do nothing and skip
         */
    }
    *out = '\0';
}

static uint32_t utf8_4b_to_utf32(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4)
{
    return ((c1 & 0x07) << 18) | ((c2 & 0x3f) << 12) | ((c3 & 0x3f) << 6) | (c4 & 0x3f);
}

// TODO: current behavior of converting UTF8 to UTF-16 has a few issues below
//
// 1. invalid trailing bytes (i.e. not b'10xxxxxx) are treated as valid trailing
//    bytes and follows normal conversion rules
// 2. invalid leading byte (b'10xxxxxx) is treated as a valid single UTF-8 byte
// 3. invalid leading byte (b'11111xxx) is treated as a valid leading byte
//    (same as b'11110xxx) for a 4-byte UTF-8 sequence
// 4. an invalid 4-byte UTF-8 sequence that translates to a codepoint < U+10000
//    will be converted as a valid UTF-16 character
//
// We keep the current behavior as is but with warnings logged, so as not to
// break compatibility.  However, this needs to be addressed later.

static jsize utf8_to_utf16_length(const char* u8str, jsize u8len)
{
    const char* const in_end = u8str + u8len;
    const char* in = u8str;
    jsize utf16_len = 0;

    while (in < in_end) {
        uint8_t c = *in;
        utf16_len++;
        if (LIKELY((c & 0x80) == 0)) {
            in++;
            continue;
        }
        if (UNLIKELY(c < 0xc0)) {
            in++;
            continue;
        }
        if (LIKELY(c < 0xe0)) {
            in += 2;
            continue;
        }
        if (LIKELY(c < 0xf0)) {
            in += 3;
            continue;
        } else {
            uint8_t c2, c3, c4;
            c2 = in[1];
            c3 = in[2];
            c4 = in[3];
            if (utf8_4b_to_utf32(c, c2, c3, c4) >= 0x10000) {
                utf16_len++;
            }
            in += 4;
            continue;
        }
    }
    if (in == in_end) {
        return utf16_len;
    }
    return 0;
}

static jchar*
utf8_to_utf16_no_null_terminator(const char* src, jsize srcLen, jchar* dst, jsize dstLen)
{
    if (src == NULL || srcLen == 0 || dstLen == 0) {
        return dst;
    }

    const char* const in_end = src + srcLen;
    const char* in = src;
    const jchar* const out_end = dst + dstLen;
    jchar* out = dst;
    uint8_t c, c2, c3, c4;
    uint32_t w;

    while (in < in_end && out < out_end) {
        c = *in++;
        if (LIKELY((c & 0x80) == 0)) {
            *out++ = (jchar) (c);
            continue;
        }
        if (UNLIKELY(c < 0xc0)) {
            ;
            *out++ = (jchar) (c);
            continue;
        }
        if (LIKELY(c < 0xe0)) {
            if (UNLIKELY(in + 1 > in_end)) {
                return out;
            }
            c2 = *in++;
            *out++ = (jchar) (((c & 0x1f) << 6) | (c2 & 0x3f));
            continue;
        }
        if (LIKELY(c < 0xf0)) {
            if (UNLIKELY(in + 2 > in_end)) {
                return out;
            }
            c2 = *in++;
            c3 = *in++;
            *out++ = (jchar) (((c & 0x0f) << 12) | ((c2 & 0x3f) << 6) | (c3 & 0x3f));
            continue;
        } else {
            if (UNLIKELY(in + 3 > in_end)) {
                return out;
            }
            if (UNLIKELY(c >= 0xf8)) {
                //error
            }
            // Multiple UTF16 characters with surrogates
            c2 = *in++;
            c3 = *in++;
            c4 = *in++;
            w = utf8_4b_to_utf32(c, c2, c3, c4);
            if (UNLIKELY(w < 0x10000)) {
                *out++ = (jchar) (w);
            } else {
                if (UNLIKELY(out + 2 > out_end)) {
                    // Ooops.... not enough room for this surrogate pair.
                    return out;
                }
                *out++ = (jchar) (((w - 0x10000) >> 10) + 0xd800);
                *out++ = (jchar) (((w - 0x10000) & 0x3ff) + 0xdc00);
            }
            continue;
        }
    }
    return out;
}

static jchar* utf8_to_utf16(const char* u8str, jsize u8len, jchar* u16str, jsize u16len)
{
    jchar* end = utf8_to_utf16_no_null_terminator(u8str, u8len, u16str, u16len - 1);
    *end = 0;
    return end;
}