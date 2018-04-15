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
#import <WCDB/Interface.h>
#import <WCDB/WCTError+Private.h>

class WCTCursorInfo : public WCDB::FTS::WCDBCursorInfo {
public:
    WCTCursorInfo(const char *input,
                  int inputLength,
                  WCDB::FTS::TokenizerInfoBase *tokenizerInfo)
        : WCDB::FTS::WCDBCursorInfo(input, inputLength, tokenizerInfo)
        , m_symbolCharacterSet(GenerateSymbolCharacterSet())
    {
    }

    WCTCursorInfo(const WCTCursorInfo &) = delete;
    WCTCursorInfo &operator=(const WCTCursorInfo &) = delete;

    ~WCTCursorInfo()
    {
        if (m_symbolCharacterSet) {
            CFRelease(m_symbolCharacterSet);
        }
    }

protected:
    CFCharacterSetRef m_symbolCharacterSet;
    static CFCharacterSetRef GenerateSymbolCharacterSet()
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

    int isSymbol(WCDB::FTS::UnicodeChar theChar, bool *result) override
    {
        if (m_symbolCharacterSet) {
            *result = CFCharacterSetIsCharacterMember(m_symbolCharacterSet, theChar);
            return SQLITE_OK;
        }
        return SQLITE_NOMEM;
    }

    int lemmatization(const char *input, int inputLength) override
    {
        int rc = WCDB::FTS::WCDBCursorInfo::lemmatization(input, inputLength);
        if (rc != SQLITE_OK) {
            return rc;
        }
        __block NSString *lemma = nil;
        NSString *string = [[NSString alloc] initWithBytes:input length:inputLength encoding:NSASCIIStringEncoding];
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
            m_lemmaBufferLength = (int) lemma.length;
            if (m_lemmaBufferLength > m_lemmaBuffer.capacity()) {
                m_lemmaBuffer.resize(lemma.length);
            }
            memcpy(m_lemmaBuffer.data(), lemma.UTF8String, m_lemmaBufferLength);
        }
        return rc;
    }
};

@implementation WCTTokenizer {
    std::shared_ptr<WCTCursorInfo> _info;
    NSString *_source;
    WCDB::SQLiteError _error;
}

- (instancetype)initWithString:(NSString *)string
{
    assert(string != nil);
    if (self = [super init]) {
        _source = [string copy];
        _info = std::shared_ptr<WCTCursorInfo>(new WCTCursorInfo(_source.UTF8String, (int) _source.length, nullptr));
        if (!_info) {
            return nil;
        }
    }
    return self;
}

- (NSArray<NSString *> *)allTokens
{
    NSMutableArray<NSString *> *tokens = [[NSMutableArray<NSString *> alloc] init];
    NSString *token;
    while ((token = [self nextToken])) {
        [tokens addObject:token];
    }
    return _error.isOK() ? tokens : nil;
}

- (NSString *)nextToken
{
    const char *token;
    int bytes;
    int startOffset;
    int endOffset;
    int position;
    _error.code = _info->step(&token, &bytes, &startOffset, &endOffset, &position);
    if (_error.code != SQLITE_OK) {
        return nil;
    }
    return [[NSString alloc] initWithBytes:token length:bytes encoding:NSUTF8StringEncoding];
    ;
}

- (WCTSQLiteError *)error
{
    return [[WCTSQLiteError alloc] initWithWCDBError:&_error];
}

+ (NSString *)name
{
    return @"WCDB";
}

+ (unsigned char *)address
{
    return WCDB::FTS::Module<void, WCTCursorInfo>::address();
}

+ (void)enroll
{
    WCDB::FTS::Modules::shared()->addAddress(self.name.UTF8String, self.address);
}

@end
