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

#include "fts3_tokenizer.h"
#include <icucompat.h>
#include <unicode/ubrk.h>
#include <unicode/unorm.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>
#include <string.h>
#include <stdio.h>
#include "mm_fts.h"
//#include <android/log.h>

#ifdef _WIN32
#include <malloc.h>
#endif


#define ROUND4(n)	(((n) + 3) & ~3)


typedef struct mm_tokenizer_t {
    sqlite3_tokenizer base;
    char locale[16];
} mm_tokenizer_t;

typedef struct mm_cursor_t {
    sqlite3_tokenizer_cursor base;
    
    UBreakIterator *iter;	// UBreakIterator for the text.
    UChar *in_buffer;		// whole input text buffer, in UTF-16, allocated altogather with mm_cursor_t.
    int *in_offset;
    int in_length;			// input text length.
    
    char *out_buffer;		// output token buffer, int UTF-8, allocated in mmtok_next.
    int out_length;			// output token buffer length.
    int token_count;
    
    int32_t ideo_start;		// ideographic unary/binary tokenizing cursor.
    int32_t ideo_end;		// ideographic unary/binary tokenizing end point.
    int ideo_state;			// 0 for unary output, -1 for invalid status.
} mm_cursor_t;
#define MINIMAL_OUT_BUFFER_LENGTH		512


static char * generate_token_printable_code(const UChar *buf, int32_t length)
{
    char *out = (char *) malloc(length * 5 + 1);
    char *pc = out;
    if (!out) return "";
    
    while (length > 0)
    {
        snprintf(pc, 6, "%04hX ", *buf);
        length--;
        buf++;
        pc += 5;
    }
    
    return out;
}


static int output_token(mm_cursor_t *cur, int32_t start, int32_t end,
        const char **ppToken, int *pnBytes, int *piStartOffset,
        int *piEndOffset, int *piPosition)
{
    UChar buf1[256];
    UChar buf2[256];
    UErrorCode status = U_ZERO_ERROR;
    int32_t result;
    int32_t length;
    
    length = end - start;
    if (length > 256) length = 256;
    result = unorm_normalize(cur->in_buffer + start, length, UNORM_NFKD, 0, buf1, sizeof(buf1) / sizeof(UChar), &status);
    // curruntly, only try fixed length buffer, failed if overflowed.
    if (U_FAILURE(status) || result > sizeof(buf1) / sizeof(UChar))
    {
        char *seq = generate_token_printable_code(cur->in_buffer + start, length);
        sqlite3_mm_set_last_error("Normalize token failed. ICU status: %d, input: %s", status, seq);
        free(seq);
        return SQLITE_ERROR;
    }
    
    length = result;
    result = u_strFoldCase(buf2, sizeof(buf2) / sizeof(UChar), buf1, length, U_FOLD_CASE_DEFAULT, &status);
    // curruntly, only try fixed length buffer, failed if overflowed.
    if (U_FAILURE(status) || result > sizeof(buf2) / sizeof(UChar))
    {
        char *seq = generate_token_printable_code(buf1, length);
        sqlite3_mm_set_last_error("FoldCase token failed. ICU status: %d, input: %s", status, seq);
        free(seq);
        return SQLITE_ERROR;
    }
    
    if (cur->out_buffer == NULL)
    {
        cur->out_buffer = (char *) sqlite3_malloc(MINIMAL_OUT_BUFFER_LENGTH * sizeof(char));
        if (!cur->out_buffer)
            return SQLITE_NOMEM;
        cur->out_length = MINIMAL_OUT_BUFFER_LENGTH;
    }
    
    length = result;
    u_strToUTF8(cur->out_buffer, cur->out_length, &result, buf2, length, &status);
    if (result > cur->out_length)
    {
        char *b = (char *) sqlite3_realloc(cur->out_buffer, result * sizeof(char));
        if (!b) return SQLITE_NOMEM;
        
        cur->out_buffer = b;
        cur->out_length = result;
        
        status = U_ZERO_ERROR;
        u_strToUTF8(cur->out_buffer, cur->out_length, &result, buf2, length, &status);
    }
    if (U_FAILURE(status) || result > cur->out_length)
    {
        char *seq = generate_token_printable_code(buf2, length);
        sqlite3_mm_set_last_error("Transform token to UTF-8 failed. ICU status: %d, input: %s", status, seq);
        free(seq);
        return SQLITE_ERROR;
    }
    
    *ppToken = cur->out_buffer;
    *pnBytes = result;
    *piStartOffset = cur->in_offset[start];
    *piEndOffset = cur->in_offset[end];
    *piPosition = cur->token_count++;
    
    //__android_log_print(ANDROID_LOG_VERBOSE, "TOKENIZER", "> %s, %d, %d, %d, %d", 
    //		*ppToken, *pnBytes, *piStartOffset, *piEndOffset, *piPosition);
    return SQLITE_OK;
}

static int find_splited_ideo_token(mm_cursor_t *cur, int32_t *start, int32_t *end)
{
    int32_t s, e;
    UChar32 c;

    if (cur->ideo_state < 0)
        return 0;
        
    if (cur->ideo_start == cur->ideo_end)
    {
        cur->ideo_state = -1;
        return 0;
    }
    
    // check UTF-16 surrogates, output 2 UChars if it's a lead surrogates, otherwise 1.
    s = cur->ideo_start;
    e = s + 1;
    c = cur->in_buffer[s];
    if (U16_IS_LEAD(c) && cur->ideo_end - s >= 2)
        e++;
    
    *start = s;
    *end = e;
    cur->ideo_start = e;
    return 1;
}


static int mmtok_create(int argc, const char * const *argv, sqlite3_tokenizer **ppTokenizer)
{
    mm_tokenizer_t *tok = sqlite3_malloc(sizeof(mm_tokenizer_t));
    if (!tok)
        return SQLITE_NOMEM;
    
    if (argc > 0)
    {
        strncpy(tok->locale, argv[0], 15);
        tok->locale[15] = 0;
    }
    else
        tok->locale[0] = 0;
        
    *ppTokenizer = (sqlite3_tokenizer *)tok;
    return SQLITE_OK;
}

static int mmtok_destroy(sqlite3_tokenizer *pTokenizer)
{
    mm_tokenizer_t *tok = (mm_tokenizer_t *)pTokenizer;
    sqlite3_free(tok);
    return SQLITE_OK;
}

static int mmtok_open(sqlite3_tokenizer *pTokenizer, const char *zInput,
        int nInput, sqlite3_tokenizer_cursor **ppCursor)
{
    mm_tokenizer_t *tok = (mm_tokenizer_t *)pTokenizer;
    mm_cursor_t *cur;
    int i_input;
    int i_output;
    
    UErrorCode status = U_ZERO_ERROR;
    int32_t dst_len;
    UChar32 c;
    
    //__android_log_print(ANDROID_LOG_VERBOSE, "TOKENIZER", "Begin: %s", zInput);
    
    if (nInput < 0)
        nInput = strlen(zInput);
        
    dst_len = ROUND4(nInput + 1);
    cur = (mm_cursor_t *) sqlite3_malloc(sizeof(mm_cursor_t)
        + dst_len * sizeof(UChar)		// in_buffer
        + (dst_len + 1) * sizeof(int)	// in_offset
        );
    if (!cur)
        return SQLITE_NOMEM;
        
    memset(cur, 0, sizeof(mm_cursor_t));
    cur->in_buffer = (UChar *)&cur[1];
    cur->in_offset = (int *)&cur->in_buffer[dst_len];
    cur->out_buffer = NULL;
    cur->out_length = 0;
    cur->token_count = 0;
    cur->ideo_start = -1;
    cur->ideo_end = -1;
    cur->ideo_state = -1;
    
    i_input = 0;
    i_output = 0;
    cur->in_offset[i_output] = i_input;
    for (;;)
    {
        if (i_input >= nInput) break;

        U8_NEXT(zInput, i_input, nInput, c);
        if (!c) break;
        if (c < 0) c = ' ';

        int is_error = 0;
        U16_APPEND(cur->in_buffer, i_output, dst_len, c, is_error);
        if (is_error)
        {
            sqlite3_free(cur);
            sqlite3_mm_set_last_error("Writing UTF-16 character failed. Code point: 0x%x", c);
            return SQLITE_ERROR;
        }
        cur->in_offset[i_output] = i_input;
    }
    
    cur->iter = ubrk_open(UBRK_WORD, tok->locale, cur->in_buffer, i_output, &status);
    if (U_FAILURE(status))
    {
        sqlite3_mm_set_last_error("Open UBreakIterator failed. ICU error code: %d", status);
        return SQLITE_ERROR;
    }
    cur->in_length = i_output;
        
    ubrk_first(cur->iter);
    *ppCursor = (sqlite3_tokenizer_cursor *)cur;
    return SQLITE_OK;
}

static int mmtok_close(sqlite3_tokenizer_cursor *pCursor)
{
    mm_cursor_t *cur = (mm_cursor_t *)pCursor;
    ubrk_close(cur->iter);
    if (cur->out_buffer)
        sqlite3_free(cur->out_buffer);
    sqlite3_free(cur);
    return SQLITE_OK;
}

static int mmtok_next(
    sqlite3_tokenizer_cursor *pCursor,  // Cursor returned by simpleOpen
    const char **ppToken,               // OUT: *ppToken is the token text
    int *pnBytes,                       // OUT: Number of bytes in token
    int *piStartOffset,                 // OUT: Starting offset of token
    int *piEndOffset,                   // OUT: Ending offset of token
    int *piPosition                     // OUT: Position integer of token
)
{
    mm_cursor_t *cur = (mm_cursor_t *)pCursor;
    int32_t start, end;
    int32_t token_type;
    
    
    // process pending ideographic token.
    if (find_splited_ideo_token(cur, &start, &end))
        return output_token(cur, start, end, ppToken, pnBytes, piStartOffset, piEndOffset, piPosition);
    
    
    start = ubrk_current(cur->iter);
    
    // find first non-NONE token.
    for (;;)
    {
        end = ubrk_next(cur->iter);
        if (end == UBRK_DONE)
        {
            sqlite3_mm_clear_error();
            return SQLITE_DONE;
        }
            
        token_type = ubrk_getRuleStatus(cur->iter);
        if (token_type >= UBRK_WORD_NONE && token_type < UBRK_WORD_NONE_LIMIT)
        {
            // look at the first character, if it's a space or ZWSP, ignore this token.
            // also ignore '*' because sqlite parser uses it as prefix operator.
            UChar32 c = cur->in_buffer[start];
            if (c == '*' || c == 0x200b || u_isspace(c))
            {
                start = end;
                continue;
            }
        }

        break;
    }
    
    // for non-IDEO tokens, just return.
    if (token_type < UBRK_WORD_IDEO || token_type >= UBRK_WORD_IDEO_LIMIT)
        return output_token(cur, start, end, ppToken, pnBytes, piStartOffset, piEndOffset, piPosition);

    // for IDEO tokens, find all suffix ideo tokens.
    for (;;)
    {
        int32_t e = ubrk_next(cur->iter);
        if (e == UBRK_DONE)
            break;
        token_type = ubrk_getRuleStatus(cur->iter);
        if (token_type < UBRK_WORD_IDEO || token_type >= UBRK_WORD_IDEO_LIMIT)
            break;
        end = e;
    }
    ubrk_isBoundary(cur->iter, end);
    
    cur->ideo_start = start;
    cur->ideo_end = end;
    cur->ideo_state = 0;
    if (find_splited_ideo_token(cur, &start, &end))
        return output_token(cur, start, end, ppToken, pnBytes, piStartOffset, piEndOffset, piPosition);
        
    sqlite3_mm_set_last_error("IDEO token found but can't output token.");
    return SQLITE_ERROR;
}


static const sqlite3_tokenizer_module mm_tokenizer_module =
{
    0,
    mmtok_create,
    mmtok_destroy,
    mmtok_open,
    mmtok_close,
    mmtok_next
};


int sqlite3_register_mm_tokenizer(sqlite3 *db)
{
    const sqlite3_tokenizer_module *module = &mm_tokenizer_module;
    sqlite3_stmt *stmt = NULL;
    int result;
    
    result = sqlite3_prepare_v2(db, "SELECT fts3_tokenizer(?, ?)", -1, &stmt, 0);
    if (result != SQLITE_OK)
        return result;
        
    sqlite3_bind_text(stmt, 1, "mmicu", -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, &module, sizeof(sqlite3_tokenizer_module *), SQLITE_STATIC);
    sqlite3_step(stmt);
    
    return sqlite3_finalize(stmt);
}
