//
// Created by sanhuazhang on 2019/9/25.
//

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

#import "Assertion.hpp"
#import "WCTDatabase+Table.h"
#import "WCTTable+Table.h"

@implementation WCTTable (Table)

- (NSString *)indexWithSuffix:(NSString *)indexSuffix
{
    return [self.tableName stringByAppendingString:indexSuffix];
}

- (BOOL)dropIndexWithSuffix:(NSString *)indexSuffix
{
    WCTRemedialAssert(indexSuffix != nil, "Index suffix can't be null.", return NO;);
    return [self.database dropIndex:[self.tableName stringByAppendingString:indexSuffix]];
}

@end
