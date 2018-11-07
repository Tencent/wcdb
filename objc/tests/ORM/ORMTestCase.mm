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

#import "ORMTestCase.h"

@implementation ORMTestCase

+ (void)initialize
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
#ifdef DEBUG
        WCTDatabase.debuggable = YES;
#else
        WCTDatabase.debuggable = NO;
#endif
    });
}

- (void)setUp
{
    [super setUp];
    self.continueAfterFailure = NO;
    [WCTDatabase globalTraceSQL:^(NSString* sql) {
        NSLog(@"SQL: %@", sql);
    }];
}

- (void)tearDown
{
    [WCTDatabase globalTraceSQL:nil];
    [super tearDown];
}

- (BOOL)checkCreateTableAndIndexSQLsAsExpected:(NSArray<NSString*>*)expected
{
    if (expected.count == 0) {
        return NO;
    }
    __block BOOL failed = NO;
    __block BOOL start = NO;

    NSMutableArray* sqls = [NSMutableArray array];
    [sqls addObject:@"BEGIN IMMEDIATE"];
    [sqls addObjectsFromArray:expected];
    [sqls addObject:@"COMMIT"];

    [self.database traceSQL:^(NSString* sql) {
        if (start && !failed) {
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                NSLog(@"Failed: %@", [TestCase hint:sql expecting:sqls.firstObject]);
                failed = YES;
            }
        }
    }];
    if (![self.database canOpen]) {
        return NO;
    }
    start = YES;
    if (![self createTable]) {
        failed = YES;
    }
    return !failed && sqls.count == 0;
}

@end
