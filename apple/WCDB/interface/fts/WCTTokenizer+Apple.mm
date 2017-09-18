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

#import "WCTTokenizer+Apple.h"
#import <WCDB/WCDB.h>
#import <WCDB/fts_module.hpp>

namespace WCDB {

namespace FTS {

#pragma mark - Module
class AppleModule {
public:
    constexpr static const char Name[] = "Apple";

private:
    static const std::nullptr_t s_register;
};

constexpr const char AppleModule::Name[];

const std::nullptr_t AppleModule::s_register = []() {
    FTS::Module<AppleModule::Name>::Register();
    return nullptr;
}();

#pragma mark - Cursor
class AppleCursorInfo {
public:
    NSString *input;
    CFStringTokenizerRef tokenizer;
    NSUInteger offset;
    int position;
    char *buffer;
    int bufferSize;
};

template <>
void *Module<AppleModule::Name>::CreateCursorInfo(const char *pInput, int nBytes)
{
    AppleCursorInfo *cursorInfo = (AppleCursorInfo *) sqlite3_malloc(sizeof(AppleCursorInfo));
    if (cursorInfo) {
        cursorInfo->input = pInput ? [NSString stringWithUTF8String:pInput] : @"";
        cursorInfo->tokenizer = CFStringTokenizerCreate(kCFAllocatorDefault, (__bridge CFStringRef) cursorInfo->input, CFRangeMake(0, cursorInfo->input.length), kCFStringTokenizerUnitWord, CFLocaleCopyCurrent());
        cursorInfo->offset = 0;
        cursorInfo->position = 0;
        cursorInfo->buffer = nullptr;
        cursorInfo->bufferSize = 0;
    }
    return cursorInfo;
}

template <>
int Module<AppleModule::Name>::CursorStep(void *info,
                                          const char **ppToken,
                                          int *pnBytes,
                                          int *piStartOffset,
                                          int *piEndOffset,
                                          int *piPosition)
{
    if (!info) {
        return SQLITE_ERROR;
    }

    AppleCursorInfo *cursorInfo = (AppleCursorInfo *) info;
    CFStringTokenizerTokenType type = CFStringTokenizerAdvanceToNextToken(cursorInfo->tokenizer);
    if (type != kCFStringTokenizerTokenNone) {
        CFRange range = CFStringTokenizerGetCurrentTokenRange(cursorInfo->tokenizer);

        CFIndex maxBufferSize = CFStringGetMaximumSizeForEncoding(range.length, kCFStringEncodingUTF8);
        if (maxBufferSize > cursorInfo->bufferSize) {
            if (cursorInfo->buffer) {
                sqlite3_free(cursorInfo->buffer);
            }
            cursorInfo->bufferSize = (int) maxBufferSize;
            cursorInfo->buffer = (char *) sqlite3_malloc(cursorInfo->bufferSize);
            if (!cursorInfo->buffer) {
                return SQLITE_NOMEM;
            }
        }
        NSUInteger used;
        BOOL result = [cursorInfo->input getBytes:cursorInfo->buffer maxLength:cursorInfo->bufferSize usedLength:&used encoding:NSUTF8StringEncoding options:NSStringEncodingConversionAllowLossy range:NSMakeRange(range.location, range.length) remainingRange:NULL];
        if (!result) {
            return SQLITE_ERROR;
        }

        *ppToken = cursorInfo->buffer;
        *piStartOffset = (int) cursorInfo->offset;
        *pnBytes = (int) used;
        *piEndOffset = (int) (cursorInfo->offset + used);
        *piPosition = cursorInfo->position;

        cursorInfo->offset += used;
        ++cursorInfo->position;

        return SQLITE_OK;
    }
    return SQLITE_DONE;
}

template <>
void Module<AppleModule::Name>::DestroyCursorInfo(void *info)
{
    if (info) {
        AppleCursorInfo *cursorInfo = (AppleCursorInfo *) info;
        if (cursorInfo->tokenizer) {
            CFRelease(cursorInfo->tokenizer);
            cursorInfo->tokenizer = nullptr;
        }
        cursorInfo->input = nil;
        if (cursorInfo->buffer) {
            sqlite3_free(cursorInfo->buffer);
            cursorInfo->buffer = nullptr;
        }
        sqlite3_free(cursorInfo);
    }
}

} //namespace FTS

} //namespace WCDB

NSString *const WCTTokenizerNameApple = @(WCDB::FTS::AppleModule::Name);
