/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Unicode.h"

#include <stddef.h>

#ifdef HAVE_WINSOCK
#undef nhtol
#undef htonl
#undef nhtos
#undef htons

#ifdef HAVE_LITTLE_ENDIAN
#define ntohl(x)                                                               \
    (((x) << 24) | (((x) >> 24) & 255) | (((x) << 8) & 0xff0000) |             \
     (((x) >> 8) & 0xff00))
#define htonl(x) ntohl(x)
#define ntohs(x) ((((x) << 8) & 0xff00) | (((x) >> 8) & 255))
#define htons(x) ntohs(x)
#else
#define ntohl(x) (x)
#define htonl(x) (x)
#define ntohs(x) (x)
#define htons(x) (x)
#endif
#else
#include <netinet/in.h>
#endif

extern "C" {

static const uint32_t kByteMask = 0x000000BF;
static const uint32_t kByteMark = 0x00000080;

// Surrogates aren't valid for UTF-32 characters, so define some
// constants that will let us screen them out.
static const uint32_t kUnicodeSurrogateHighStart = 0x0000D800;
static const uint32_t kUnicodeSurrogateHighEnd = 0x0000DBFF;
static const uint32_t kUnicodeSurrogateLowStart = 0x0000DC00;
static const uint32_t kUnicodeSurrogateLowEnd = 0x0000DFFF;
static const uint32_t kUnicodeSurrogateStart = kUnicodeSurrogateHighStart;
static const uint32_t kUnicodeSurrogateEnd = kUnicodeSurrogateLowEnd;
static const uint32_t kUnicodeMaxCodepoint = 0x0010FFFF;

// Mask used to set appropriate bits in first byte of UTF-8 sequence,
// indexed by number of bytes in the sequence.
// 0xxxxxxx
// -> (00-7f) 7bit. Bit mask for the first byte is 0x00000000
// 110yyyyx 10xxxxxx
// -> (c0-df)(80-bf) 11bit. Bit mask is 0x000000C0
// 1110yyyy 10yxxxxx 10xxxxxx
// -> (e0-ef)(80-bf)(80-bf) 16bit. Bit mask is 0x000000E0
// 11110yyy 10yyxxxx 10xxxxxx 10xxxxxx
// -> (f0-f7)(80-bf)(80-bf)(80-bf) 21bit. Bit mask is 0x000000F0
static const uint32_t kFirstByteMark[] = {0x00000000, 0x00000000, 0x000000C0,
                                          0x000000E0, 0x000000F0};

// --------------------------------------------------------------------------
// UTF-32
// --------------------------------------------------------------------------

/**
     * Return number of UTF-8 bytes required for the character. If the character
     * is invalid, return size of 0.
     */
static inline size_t utf32_codepoint_utf8_length(uint32_t srcChar)
{
    // Figure out how many bytes the result will require.
    if (srcChar < 0x00000080) {
        return 1;
    } else if (srcChar < 0x00000800) {
        return 2;
    } else if (srcChar < 0x00010000) {
        if ((srcChar < kUnicodeSurrogateStart) ||
            (srcChar > kUnicodeSurrogateEnd)) {
            return 3;
        } else {
            // Surrogates are invalid UTF-32 characters.
            return 0;
        }
    }
    // Max code point for Unicode is 0x0010FFFF.
    else if (srcChar <= kUnicodeMaxCodepoint) {
        return 4;
    } else {
        // Invalid UTF-32 character.
        return 0;
    }
}

// Write out the source character to <dstP>.

static inline void
utf32_codepoint_to_utf8(uint8_t *dstP, uint32_t srcChar, size_t bytes)
{
    dstP += bytes;
    switch (bytes) { /* note: everything falls through. */
        case 4:
            *--dstP = (uint8_t)((srcChar | kByteMark) & kByteMask);
            srcChar >>= 6;
        case 3:
            *--dstP = (uint8_t)((srcChar | kByteMark) & kByteMask);
            srcChar >>= 6;
        case 2:
            *--dstP = (uint8_t)((srcChar | kByteMark) & kByteMask);
            srcChar >>= 6;
        case 1:
            *--dstP = (uint8_t)(srcChar | kFirstByteMark[bytes]);
    }
}

size_t strlen32(const uint32_t *s)
{
    const uint32_t *ss = s;
    while (*ss)
        ss++;
    return ss - s;
}

size_t strnlen32(const uint32_t *s, size_t maxlen)
{
    const uint32_t *ss = s;
    while ((maxlen > 0) && *ss) {
        ss++;
        maxlen--;
    }
    return ss - s;
}

static inline int32_t utf32_at_internal(const char *cur, size_t *num_read)
{
    const char first_char = *cur;
    if ((first_char & 0x80) == 0) { // ASCII
        *num_read = 1;
        return *cur;
    }
    cur++;
    uint32_t mask, to_ignore_mask;
    size_t num_to_read = 0;
    uint32_t utf32 = first_char;
    for (num_to_read = 1, mask = 0x40, to_ignore_mask = 0xFFFFFF80;
         (first_char & mask);
         num_to_read++, to_ignore_mask |= mask, mask >>= 1) {
        // 0x3F == 00111111
        utf32 = (utf32 << 6) + (*cur++ & 0x3F);
    }
    to_ignore_mask |= mask;
    utf32 &= ~(to_ignore_mask << (6 * (num_to_read - 1)));

    *num_read = num_to_read;
    return static_cast<int32_t>(utf32);
}

int32_t utf32_from_utf8_at(const char *src,
                           size_t src_len,
                           size_t index,
                           size_t *next_index)
{
    if (index >= src_len) {
        return -1;
    }
    size_t dummy_index;
    if (next_index == NULL) {
        next_index = &dummy_index;
    }
    size_t num_read;
    int32_t ret = utf32_at_internal(src + index, &num_read);
    if (ret >= 0) {
        *next_index = index + num_read;
    }

    return ret;
}

ssize_t utf32_to_utf8_length(const uint32_t *src, size_t src_len)
{
    if (src == NULL || src_len == 0) {
        return -1;
    }

    size_t ret = 0;
    const uint32_t *end = src + src_len;
    while (src < end) {
        ret += utf32_codepoint_utf8_length(*src++);
    }
    return ret;
}

void utf32_to_utf8(const uint32_t *src, size_t src_len, char *dst)
{
    if (src == NULL || src_len == 0 || dst == NULL) {
        return;
    }

    const uint32_t *cur_utf32 = src;
    const uint32_t *end_utf32 = src + src_len;
    char *cur = dst;
    while (cur_utf32 < end_utf32) {
        size_t len = utf32_codepoint_utf8_length(*cur_utf32);
        utf32_codepoint_to_utf8((uint8_t *) cur, *cur_utf32++, len);
        cur += len;
    }
    *cur = '\0';
}

// --------------------------------------------------------------------------
// UTF-16
// --------------------------------------------------------------------------

int strcmp16(const uint16_t *s1, const uint16_t *s2)
{
    uint16_t ch;
    int d = 0;

    while (1) {
        d = (int) (ch = *s1++) - (int) *s2++;
        if (d || !ch)
            break;
    }

    return d;
}

int strncmp16(const uint16_t *s1, const uint16_t *s2, size_t n)
{
    uint16_t ch;
    int d = 0;

    while (n--) {
        d = (int) (ch = *s1++) - (int) *s2++;
        if (d || !ch)
            break;
    }

    return d;
}

uint16_t *strcpy16(uint16_t *dst, const uint16_t *src)
{
    uint16_t *q = dst;
    const uint16_t *p = src;
    uint16_t ch;

    do {
        *q++ = ch = *p++;
    } while (ch);

    return dst;
}

size_t strlen16(const uint16_t *s)
{
    const uint16_t *ss = s;
    while (*ss)
        ss++;
    return ss - s;
}

uint16_t *strncpy16(uint16_t *dst, const uint16_t *src, size_t n)
{
    uint16_t *q = dst;
    const uint16_t *p = src;
    char ch;

    while (n) {
        n--;
        *q++ = ch = *p++;
        if (!ch)
            break;
    }

    *q = 0;

    return dst;
}

size_t strnlen16(const uint16_t *s, size_t maxlen)
{
    const uint16_t *ss = s;

    /* Important: the maxlen test must precede the reference through ss;
           since the byte beyond the maximum may segfault */
    while ((maxlen > 0) && *ss) {
        ss++;
        maxlen--;
    }
    return ss - s;
}

int strzcmp16(const uint16_t *s1, size_t n1, const uint16_t *s2, size_t n2)
{
    const uint16_t *e1 = s1 + n1;
    const uint16_t *e2 = s2 + n2;

    while (s1 < e1 && s2 < e2) {
        const int d = (int) *s1++ - (int) *s2++;
        if (d) {
            return d;
        }
    }

    return n1 < n2 ? (0 - (int) *s2) : (n1 > n2 ? ((int) *s1 - 0) : 0);
}

int strzcmp16_h_n(const uint16_t *s1H,
                  size_t n1,
                  const uint16_t *s2N,
                  size_t n2)
{
    const uint16_t *e1 = s1H + n1;
    const uint16_t *e2 = s2N + n2;

    while (s1H < e1 && s2N < e2) {
        const uint16_t c2 = ntohs(*s2N);
        const int d = (int) *s1H++ - (int) c2;
        s2N++;
        if (d) {
            return d;
        }
    }

    return n1 < n2 ? (0 - (int) ntohs(*s2N)) : (n1 > n2 ? ((int) *s1H - 0) : 0);
}

void utf16_to_utf8(const uint16_t *src, size_t src_len, char *dst)
{
    if (src == NULL || src_len == 0 || dst == NULL) {
        return;
    }

    const uint16_t *cur_utf16 = src;
    const uint16_t *const end_utf16 = src + src_len;
    char *cur = dst;
    while (cur_utf16 < end_utf16) {
        uint32_t utf32;
        // surrogate pairs
        if ((*cur_utf16 & 0xFC00) == 0xD800) {
            utf32 = (*cur_utf16++ - 0xD800) << 10;
            utf32 |= *cur_utf16++ - 0xDC00;
            utf32 += 0x10000;
        } else {
            utf32 = (uint32_t) *cur_utf16++;
        }
        const size_t len = utf32_codepoint_utf8_length(utf32);
        utf32_codepoint_to_utf8((uint8_t *) cur, utf32, len);
        cur += len;
    }
    *cur = '\0';
}

// --------------------------------------------------------------------------
// UTF-8
// --------------------------------------------------------------------------

ssize_t utf8_length(const char *src)
{
    const char *cur = src;
    size_t ret = 0;
    while (*cur != '\0') {
        const char first_char = *cur++;
        if ((first_char & 0x80) == 0) { // ASCII
            ret += 1;
            continue;
        }
        // (UTF-8's character must not be like 10xxxxxx,
        //  but 110xxxxx, 1110xxxx, ... or 1111110x)
        if ((first_char & 0x40) == 0) {
            return -1;
        }

        int32_t mask, to_ignore_mask;
        size_t num_to_read = 0;
        uint32_t utf32 = 0;
        for (num_to_read = 1, mask = 0x40, to_ignore_mask = 0x80;
             num_to_read < 5 && (first_char & mask);
             num_to_read++, to_ignore_mask |= mask, mask >>= 1) {
            if ((*cur & 0xC0) != 0x80) { // must be 10xxxxxx
                return -1;
            }
            // 0x3F == 00111111
            utf32 = (utf32 << 6) + (*cur++ & 0x3F);
        }
        // "first_char" must be (110xxxxx - 11110xxx)
        if (num_to_read == 5) {
            return -1;
        }
        to_ignore_mask |= mask;
        utf32 |= ((~to_ignore_mask) & first_char) << (6 * (num_to_read - 1));
        if (utf32 > kUnicodeMaxCodepoint) {
            return -1;
        }

        ret += num_to_read;
    }
    return ret;
}

ssize_t utf16_to_utf8_length(const uint16_t *src, size_t src_len)
{
    if (src == NULL || src_len == 0) {
        return -1;
    }

    size_t ret = 0;
    const uint16_t *const end = src + src_len;
    while (src < end) {
        if ((*src & 0xFC00) == 0xD800 && (src + 1) < end &&
            (*++src & 0xFC00) == 0xDC00) {
            // surrogate pairs are always 4 bytes.
            ret += 4;
            src++;
        } else {
            ret += utf32_codepoint_utf8_length((uint32_t) *src++);
        }
    }
    return ret;
}

/**
     * Returns 1-4 based on the number of leading bits.
     *
     * 1111 -> 4
     * 1110 -> 3
     * 110x -> 2
     * 10xx -> 1
     * 0xxx -> 1
     */
static inline size_t utf8_codepoint_len(uint8_t ch)
{
    return ((0xe5000000 >> ((ch >> 3) & 0x1e)) & 3) + 1;
}

static inline void utf8_shift_and_mask(uint32_t *codePoint, const uint8_t byte)
{
    *codePoint <<= 6;
    *codePoint |= 0x3F & byte;
}

size_t utf8_to_utf32_length(const char *src, size_t src_len)
{
    if (src == NULL || src_len == 0) {
        return 0;
    }
    size_t ret = 0;
    const char *cur;
    const char *end;
    size_t num_to_skip;
    for (cur = src, end = src + src_len, num_to_skip = 1; cur < end;
         cur += num_to_skip, ret++) {
        const char first_char = *cur;
        num_to_skip = 1;
        if ((first_char & 0x80) == 0) { // ASCII
            continue;
        }
        int32_t mask;

        for (mask = 0x40; (first_char & mask); num_to_skip++, mask >>= 1) {
        }
    }
    return ret;
}

void utf8_to_utf32(const char *src, size_t src_len, uint32_t *dst)
{
    if (src == NULL || src_len == 0 || dst == NULL) {
        return;
    }

    const char *cur = src;
    const char *const end = src + src_len;
    uint32_t *cur_utf32 = dst;
    while (cur < end) {
        size_t num_read;
        *cur_utf32++ = static_cast<uint32_t>(utf32_at_internal(cur, &num_read));
        cur += num_read;
    }
    *cur_utf32 = 0;
}

static inline uint32_t utf8_to_utf32_codepoint(const uint8_t *src,
                                               size_t length)
{
    uint32_t unicode;

    switch (length) {
        case 1:
            return src[0];
        case 2:
            unicode = src[0] & 0x1f;
            utf8_shift_and_mask(&unicode, src[1]);
            return unicode;
        case 3:
            unicode = src[0] & 0x0f;
            utf8_shift_and_mask(&unicode, src[1]);
            utf8_shift_and_mask(&unicode, src[2]);
            return unicode;
        case 4:
            unicode = src[0] & 0x07;
            utf8_shift_and_mask(&unicode, src[1]);
            utf8_shift_and_mask(&unicode, src[2]);
            utf8_shift_and_mask(&unicode, src[3]);
            return unicode;
        default:
            return 0xffff;
    }

    //printf("Char at %p: len=%d, utf-16=%p\n", src, length, (void*)result);
}

ssize_t utf8_to_utf16_length(const uint8_t *u8str, size_t u8len)
{
    const uint8_t *const u8end = u8str + u8len;
    const uint8_t *u8cur = u8str;

    /* Validate that the UTF-8 is the correct len */
    size_t u16measuredLen = 0;
    while (u8cur < u8end) {
        u16measuredLen++;
        int u8charLen = utf8_codepoint_len(*u8cur);
        uint32_t codepoint = utf8_to_utf32_codepoint(u8cur, u8charLen);
        if (codepoint > 0xFFFF)
            u16measuredLen++; // this will be a surrogate pair in utf16
        u8cur += u8charLen;
    }

    /**
         * Make sure that we ended where we thought we would and the output UTF-16
         * will be exactly how long we were told it would be.
         */
    if (u8cur != u8end) {
        return -1;
    }

    return u16measuredLen;
}

uint16_t *utf8_to_utf16_no_null_terminator(const uint8_t *u8str,
                                           size_t u8len,
                                           uint16_t *u16str)
{
    const uint8_t *const u8end = u8str + u8len;
    const uint8_t *u8cur = u8str;
    uint16_t *u16cur = u16str;

    while (u8cur < u8end) {
        size_t u8len = utf8_codepoint_len(*u8cur);
        uint32_t codepoint = utf8_to_utf32_codepoint(u8cur, u8len);

        // Convert the UTF32 codepoint to one or more UTF16 codepoints
        if (codepoint <= 0xFFFF) {
            // Single UTF16 character
            *u16cur++ = (uint16_t) codepoint;
        } else {
            // Multiple UTF16 characters with surrogates
            codepoint = codepoint - 0x10000;
            *u16cur++ = (uint16_t)((codepoint >> 10) + 0xD800);
            *u16cur++ = (uint16_t)((codepoint & 0x3FF) + 0xDC00);
        }

        u8cur += u8len;
    }
    return u16cur;
}

void utf8_to_utf16(const uint8_t *u8str, size_t u8len, uint16_t *u16str)
{
    uint16_t *end = utf8_to_utf16_no_null_terminator(u8str, u8len, u16str);
    *end = 0;
}
}
