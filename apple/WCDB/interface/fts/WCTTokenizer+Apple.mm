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

#import <WCDB/WCDB.h>
#import <WCDB/WCTTokenizer+Apple.h>
#import <WCDB/fts_module.hpp>

namespace WCDB {

namespace FTS {

#pragma mark - Tokenizer
class AppleTokenizerInfo : public TokenizerInfoBase {
public:
    AppleTokenizerInfo(int argc, const char *const *argv)
        : TokenizerInfoBase(argc, argv)
    {
    }
    ~AppleTokenizerInfo()
    {
    }

protected:
};

#pragma mark - Cursor
class AppleCursorInfo : public CursorInfoBase {
public:
    AppleCursorInfo(const char *input, int bytes, TokenizerInfoBase *tokenizerInfo)
        : CursorInfoBase(input, bytes, tokenizerInfo)
        , m_input(input ? [NSString stringWithUTF8String:input] : @"")
        , m_tokenizer(CFStringTokenizerCreate(kCFAllocatorDefault, (__bridge CFStringRef) m_input, CFRangeMake(0, m_input.length), kCFStringTokenizerUnitWord, CFLocaleCopyCurrent()))
        , m_offset(0)
        , m_position(0)
        , m_buffer(0)
        , m_bufferSize(0)
    {
    }

    ~AppleCursorInfo()
    {
        if (m_tokenizer) {
            CFRelease(m_tokenizer);
        }
        if (m_buffer) {
            sqlite3_free(m_buffer);
        }
    }

    virtual int step(const char **ppToken,
                     int *pnBytes,
                     int *piStartOffset,
                     int *piEndOffset,
                     int *piPosition) override
    {
        CFStringTokenizerTokenType type = CFStringTokenizerAdvanceToNextToken(m_tokenizer);
        if (type != kCFStringTokenizerTokenNone) {
            CFRange range = CFStringTokenizerGetCurrentTokenRange(m_tokenizer);

            CFIndex maxBufferSize = CFStringGetMaximumSizeForEncoding(range.length, kCFStringEncodingUTF8);
            if (maxBufferSize > m_bufferSize) {
                if (m_buffer) {
                    sqlite3_free(m_buffer);
                }
                m_bufferSize = (int) maxBufferSize;
                m_buffer = (char *) sqlite3_malloc(m_bufferSize);
                if (!m_buffer) {
                    return SQLITE_NOMEM;
                }
            }
            NSUInteger used;
            BOOL result = [m_input getBytes:m_buffer maxLength:m_bufferSize usedLength:&used encoding:NSUTF8StringEncoding options:NSStringEncodingConversionAllowLossy range:NSMakeRange(range.location, range.length) remainingRange:NULL];
            if (!result) {
                return SQLITE_ERROR;
            }

            *ppToken = m_buffer;
            *piStartOffset = (int) m_offset;
            *pnBytes = (int) used;
            *piEndOffset = (int) (m_offset + used);
            *piPosition = m_position;

            m_offset += used;
            ++m_position;

            return SQLITE_OK;
        }
        return SQLITE_DONE;
    }

protected:
    NSString *m_input;
    CFStringTokenizerRef m_tokenizer;
    NSUInteger m_offset;
    int m_position;
    char *m_buffer;
    int m_bufferSize;
};

#pragma mark - Module
class AppleModule {
public:
    constexpr static const char Name[] = "Apple";

private:
    static const std::nullptr_t s_register;
};

constexpr const char AppleModule::Name[];

const std::nullptr_t AppleModule::s_register = []() {
    FTS::Module<AppleModule::Name, AppleTokenizerInfo, AppleCursorInfo>::Register();
    return nullptr;
}();

} //namespace FTS

} //namespace WCDB

NSString *const WCTTokenizerNameApple = @(WCDB::FTS::AppleModule::Name);
