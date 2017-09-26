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

#import <CoreFoundation/CoreFoundation.h>
#import <WCDB/WCTTokenizer+WCDB.h>
#import <WCDB/fts_module.hpp>
#import <WCDB/tokenizer.hpp>

namespace WCDB {

namespace FTS {

#pragma mark - Cursor
class WCTCursorInfo : public WCDBCursorInfo {
public:
    WCTCursorInfo(const char *input,
                  int bytes,
                  TokenizerInfoBase *tokenizerInfo);
    ~WCTCursorInfo();

protected:
    CFCharacterSetRef m_symbolCharacterSet;
    static CFCharacterSetRef GenerateSymbolCharacterSet();
    virtual int isSymbol(UnicodeChar theChar, bool *result) override;
    virtual int lemmatization(const char *input, int bytes) override;
};

WCTCursorInfo::WCTCursorInfo(const char *input,
                             int bytes,
                             TokenizerInfoBase *tokenizerInfo)
    : WCDBCursorInfo(input, bytes, tokenizerInfo)
    , m_symbolCharacterSet(GenerateSymbolCharacterSet())
{
}

WCTCursorInfo::~WCTCursorInfo()
{
    if (m_symbolCharacterSet) {
        CFRelease(m_symbolCharacterSet);
    }
}

CFCharacterSetRef WCTCursorInfo::GenerateSymbolCharacterSet()
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

int WCTCursorInfo::isSymbol(UnicodeChar theChar, bool *result)
{
    if (m_symbolCharacterSet) {
        *result = CFCharacterSetIsCharacterMember(m_symbolCharacterSet, theChar);
        return SQLITE_OK;
    }
    return SQLITE_NOMEM;
}

int WCTCursorInfo::lemmatization(const char *input, int bytes)
{
    int rc = WCDBCursorInfo::lemmatization(input, bytes);
    if (rc != SQLITE_OK) {
        return rc;
    }
    __block NSString *lemma = nil;
    NSString *string = [[NSString alloc] initWithBytes:input length:bytes encoding:NSASCIIStringEncoding];
    NSDictionary *languageMap = @{ @"Latn" : @[ @"en" ] };
    [string enumerateLinguisticTagsInRange:NSMakeRange(0, string.length)
                                    scheme:NSLinguisticTagSchemeLemma
                                   options:NSLinguisticTaggerOmitWhitespace
                               orthography:[NSOrthography orthographyWithDominantScript:@"Latn" languageMap:languageMap]
                                usingBlock:^(NSString *tag, NSRange tokenRange, NSRange sentenceRange, BOOL *stop) {
                                  lemma = tag.lowercaseString;
                                  *stop = YES;
                                }];
    if (lemma.length > 0 && [lemma caseInsensitiveCompare:string] != NSOrderedSame) {
        rc = setLemmaBuffer(lemma.UTF8String, (int) lemma.length);
    }
    return rc;
}

#pragma mark - Module
class WCTModule {
public:
    constexpr static const char Name[] = "WCDB";

private:
    static const std::nullptr_t s_register;
};

#pragma mark - Module
constexpr const char WCTModule::Name[];

const std::nullptr_t WCTModule::s_register = []() {
    Module<WCTModule::Name, void,
           WCTCursorInfo>::Register();
    return nullptr;
}();

} //namespace WCDB

} //namespace FTS

NSString *const WCTTokenizerNameWCDB = @(WCDB::FTS::WCTModule::Name);
