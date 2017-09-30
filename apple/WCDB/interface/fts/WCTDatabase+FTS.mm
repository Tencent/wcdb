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

#import <WCDB/WCTDatabase+FTS.h>
#import <WCDB/WCTDatabase+Private.h>

NSString *const WCTModuleNameFTS3 = @"fts3";

@implementation WCTDatabase (FTS)

- (void)setTokenizer:(NSString *)tokenizeName
{
    if (tokenizeName) {
        _database->setTokenizes({tokenizeName.UTF8String});
    }
}

- (void)setTokenizers:(NSArray<NSString *> *)tokenizeNames
{
    std::list<std::string> theTokenizeNames;
    for (NSString *tokenizeName in tokenizeNames) {
        theTokenizeNames.push_back(tokenizeName.UTF8String);
    }
    _database->setTokenizes(theTokenizeNames);
}

@end
