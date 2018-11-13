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

#import "SingleDatabaseTestCase.h"

@implementation SingleDatabaseTestCase

- (void)setUp
{
    [super setUp];
    [self refreshDirectory];
    _database = [[WCTDatabase alloc] initWithPath:self.path];
}

- (void)tearDown
{
    [_database close];
    [_database invalidate];
    _database = nil;
    [self cleanDirectory];
    [super tearDown];
}

- (NSString*)path
{
    return [self.directory stringByAppendingPathComponent:@"testDatabase"];
}

- (BOOL)checkAllSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedByOperation:(BOOL (^)())block
{
    BOOL result = NO;
    do {
        __block BOOL trace = NO;
        if (![expectedSQLs isKindOfClass:NSArray.class]
            || expectedSQLs.count == 0
            || block == nil) {
            TESTCASE_FAILED
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (!trace) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                NSLog(@"Failed: %@", [TestCase hint:sql expecting:sqls.firstObject]);
                trace = NO;
                TESTCASE_FAILED
            }
        }];
        if (![self.database canOpen]) {
            TESTCASE_FAILED
            break;
        }

        trace = YES;
        if (!block() || sqls.count != 0) {
            TESTCASE_FAILED
            break;
        }
        trace = NO;
        result = YES;
    } while (false);
    [self.database traceSQL:nil];
    return result;
}

- (BOOL)checkSomeSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedByOperation:(BOOL (^)())block
{
    BOOL result = NO;
    do {
        __block BOOL trace = NO;
        if (![expectedSQLs isKindOfClass:NSArray.class]
            || expectedSQLs.count == 0
            || block == nil) {
            TESTCASE_FAILED
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (!trace || sqls.count == 0) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                NSLog(@"Failed: %@", [TestCase hint:sql expecting:sqls.firstObject]);
                trace = NO;
                TESTCASE_FAILED
            }
        }];
        if (![self.database canOpen]) {
            TESTCASE_FAILED
            break;
        }

        trace = YES;
        if (!block() || sqls.count != 0) {
            TESTCASE_FAILED
            break;
        }
        trace = NO;
        result = YES;
    } while (false);
    [self.database traceSQL:nil];
    return result;
}

@end
