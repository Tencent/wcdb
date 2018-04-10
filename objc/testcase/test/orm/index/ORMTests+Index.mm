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

#import "ORMIndex+WCTTableCoding.h"
#import "ORMIndex.h"
#import "ORMMultiIndex+WCTTableCoding.h"
#import "ORMMultiIndex.h"
#import "ORMTestCase.h"
#import "ORMUniqueIndex+WCTTableCoding.h"
#import "ORMUniqueIndex.h"

@interface ORMTests_Index : ORMTestCase

@end

@implementation ORMTests_Index

- (void)test_index
{
    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index_1"];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX IF NOT EXISTS %@ ON %@(variable1)", indexName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index_2"];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX IF NOT EXISTS %@ ON %@(variable2 ASC)", indexName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index_3"];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX IF NOT EXISTS %@ ON %@(variable3 DESC)", indexName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMIndex.class]);
}

- (void)test_multi_index
{
    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index"];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX IF NOT EXISTS %@ ON %@(variable1, variable2 ASC, variable3 DESC)", indexName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMMultiIndex.class]);
}

- (void)test_unique_index
{
    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index_1"];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE UNIQUE INDEX IF NOT EXISTS %@ ON %@(variable1)", indexName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index_2"];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE UNIQUE INDEX IF NOT EXISTS %@ ON %@(variable2 ASC)", indexName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index_3"];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE UNIQUE INDEX IF NOT EXISTS %@ ON %@(variable3 DESC)", indexName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMUniqueIndex.class]);
}

@end
