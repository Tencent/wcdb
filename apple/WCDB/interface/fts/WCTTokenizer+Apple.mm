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

#ifndef WCDB_OMIT_TOKENIZE_APPLE

#import <WCDB/WCDB.h>
#import <WCDB/WCTTokenizer+Apple.h>
#import <WCDB/fts_module.hpp>
#import <vector>

namespace WCDB {

namespace FTS {

#pragma mark - Cursor
class AppleCursorInfo : public CursorInfoBase {
public:
    AppleCursorInfo(const char *input, int inputLength, TokenizerInfoBase *tokenizerInfo)
        : CursorInfoBase(input, inputLength, tokenizerInfo)
        , m_input(input ? [[NSString alloc] initWithBytes:input length:inputLength encoding:NSUTF8StringEncoding] : @"")
        , m_tokenizer(m_input ? CFStringTokenizerCreate(kCFAllocatorDefault, (__bridge CFStringRef) m_input, CFRangeMake(0, m_input.length), kCFStringTokenizerUnitWord, CFLocaleCopyCurrent()) : NULL)
        , m_offset(0)
        , m_position(0)
    {
    }

    ~AppleCursorInfo()
    {
        if (m_tokenizer) {
            CFRelease(m_tokenizer);
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
            if (maxBufferSize > m_buffer.capacity()) {
                m_buffer.resize(maxBufferSize);
            }
            NSUInteger used;
            BOOL result = [m_input getBytes:m_buffer.data() maxLength:m_buffer.capacity() usedLength:&used encoding:NSUTF8StringEncoding options:NSStringEncodingConversionAllowLossy range:NSMakeRange(range.location, range.length) remainingRange:NULL];
            if (!result) {
                return SQLITE_ERROR;
            }

            *ppToken = m_buffer.data();
            *piStartOffset = (int) m_offset;
            *pnBytes = (int) used;
            *piEndOffset = (int) (m_offset + used);
            *piPosition = m_position++;

            m_offset += used;

            printf("t %s\n", std::string(*ppToken, *pnBytes).c_str());

            return SQLITE_OK;
        }
        return SQLITE_DONE;
    }

protected:
    NSString *m_input;
    CFStringTokenizerRef m_tokenizer;
    NSUInteger m_offset;
    int m_position;
    std::vector<char> m_buffer;
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
    FTS::Module<AppleModule::Name, void, AppleCursorInfo>::Register();
    return nullptr;
}();

} //namespace FTS

} //namespace WCDB

NSString *const WCTTokenizerNameApple = @(WCDB::FTS::AppleModule::Name);

#endif //WCDB_OMIT_TOKENIZE_APPLE
