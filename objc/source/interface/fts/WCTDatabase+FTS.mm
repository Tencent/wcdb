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
#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>

WCTTokenizer const WCTTokenizerSimple = @"simple";
WCTTokenizer const WCTTokenizerPorter = @"porter";
WCTTokenizer const WCTTokenizerICU = @"icu";
WCTTokenizer const WCTTokenizerUnicode61 = @"unicode61";
WCTTokenizer const WCTTokenizerWCDB = @"WCDB";

@implementation WCTDatabase (FTS)

- (void)setTokenizer:(NSString *)tokenizerName
{
    WCTRemedialAssert(tokenizerName, "Tokenizer name can't be null.", return;)
    _database->setConfig(WCDB::Core::tokenizeConfigName,
                         WCDB::Core::shared()->tokenizeConfig({ tokenizerName.cppString }),
                         WCDB::Configs::Priority::Higher);
}

- (void)setTokenizers:(NSArray<NSString *> *)tokenizerNames
{
    WCTRemedialAssert(tokenizerNames, "Tokenizers can't be null.", return;)
    std::list<std::string>
    theTokenizeNames;
    for (NSString *tokenizerName in tokenizerNames) {
        theTokenizeNames.push_back(tokenizerName.cppString);
    }
    _database->setConfig(WCDB::Core::tokenizeConfigName,
                         WCDB::Core::shared()->tokenizeConfig(theTokenizeNames),
                         WCDB::Configs::Priority::Higher);
}

@end
