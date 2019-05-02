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

#import <WCDB/Assertion.hpp>
#import <WCDB/Error.hpp>
#import <WCDB/WCTOneOrBinaryTokenizer.h>

WCTOneOrBinaryTokenizerCursorInfo::WCTOneOrBinaryTokenizerCursorInfo(const char *input,
                                                                     int inputLength,
                                                                     WCDB::AbstractTokenizerInfo *tokenizerInfo)
: WCDB::OneOrBinaryTokenizerCursorInfo(input, inputLength, tokenizerInfo)
, m_symbolCharacterSet(generateSymbolCharacterSet())
{
}

WCTOneOrBinaryTokenizerCursorInfo::~WCTOneOrBinaryTokenizerCursorInfo()
{
    if (m_symbolCharacterSet != nil) {
        CFRelease(m_symbolCharacterSet);
        m_symbolCharacterSet = nil;
    }
}

CFCharacterSetRef WCTOneOrBinaryTokenizerCursorInfo::generateSymbolCharacterSet()
{
    //Code: Cc, Cf, Z*, U000A ~ U000D, U0085, M*, P*, S* and illegal character set
    CFMutableCharacterSetRef characterSetRef = CFCharacterSetCreateMutable(CFAllocatorGetDefault());
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetControl));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetNonBase));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetPunctuation));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetSymbol));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetIllegal));
    return characterSetRef;
}

std::pair<int, bool> WCTOneOrBinaryTokenizerCursorInfo::isSymbol(UnicodeChar theChar) const
{
    bool symbol = false;
    WCDB::Error::Code code = WCDB::Error::Code::NoMemory;
    if (m_symbolCharacterSet != nil) {
        symbol = CFCharacterSetIsCharacterMember(m_symbolCharacterSet, theChar);
        code = WCDB::Error::Code::OK;
    }
    return { WCDB::Error::c2rc(code), symbol };
}
