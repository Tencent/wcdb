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

#import "SingleTableTestCase.h"

@implementation SingleTableTestCase

- (void)setUp
{
    [super setUp];
    _tableName = @"testTable";
}

- (WCTTable*)createTable
{
    if (_tableClass == nil) {
        return nil;
    }
    if (![self.database createTableAndIndexes:_tableName withClass:_tableClass]) {
        return nil;
    }
    return [self.database getTable:_tableName withClass:_tableClass];
}

- (WCTTable*)createVirtualTable
{
    if (_tableClass == nil) {
        return nil;
    }
    if (![self.database createVirtualTable:_tableName withClass:_tableClass]) {
        return nil;
    }
    return [self.database getTable:_tableName withClass:_tableClass];
}

@end
