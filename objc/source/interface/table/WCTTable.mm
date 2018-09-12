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
#import <WCDB/WCDB.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTTable+Private.h>

@implementation WCTTable

@synthesize tableName = _tableName;
@synthesize cls = _class;

- (instancetype)initWithCore:(WCTCore *)core
                andTableName:(NSString *)tableName
                    andClass:(Class<WCTTableCoding>)cls
{
    WCTInnerAssert(cls && tableName);
    if (self = [super initWithCore:core]) {
        _tableName = tableName;
        _class = cls;
    }
    return self;
}

- (NSString *)tableName
{
    return _tableName;
}

- (WCTDatabase *)getDatabase
{
    return [[WCTDatabase alloc] initWithDatabase:_database];
}

- (WCTError *)error
{
    return [[WCTError alloc] initWithError:_database->getThreadedError()];
}

@end
