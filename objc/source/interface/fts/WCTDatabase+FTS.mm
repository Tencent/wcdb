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

#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>

@implementation WCTDatabase (FTS)

+ (void)initialize
{
    [WCTTokenizer enroll];
}

- (void)setTokenizer:(NSString *)tokenizerName
{
    _database->setTokenizes({tokenizerName.UTF8String});
}

- (void)setTokenizers:(NSArray<NSString *> *)tokenizerNames
{
    std::list<std::string> theTokenizeNames;
    for (NSString *tokenizerName in tokenizerNames) {
        theTokenizeNames.push_back(tokenizerName.UTF8String);
    }
    _database->setTokenizes(theTokenizeNames);
}

@end
