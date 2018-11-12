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

#import "ConvenientTestCase.h"

typedef NS_ENUM(NSUInteger, ConvenientTestCaseState) {
    ConvenientTestCaseStateNotStarted,
    ConvenientTestCaseStateTestingSQL,
    ConvenientTestCaseStateTestingValues,
    ConvenientTestCaseStateTested,
    ConvenientTestCaseStateFailed,
};

@implementation ConvenientTestCase

- (void)setUp
{
    [super setUp];

    self.tableClass = TestCaseObject.class;
    _table = [self createTable];
    XCTAssertNotNil(_table);

    _object1 = [[TestCaseObject alloc] init];
    _object1.identifier = 1;
    _object1.content = @"object1";

    _object2 = [[TestCaseObject alloc] init];
    _object2.identifier = 2;
    _object2.content = @"object2";

    _objects = @[ _object1, _object2 ];

    XCTAssertTrue([_table insertObjects:_objects]);
}

- (BOOL)checkObject:(TestCaseObject*)object
                     andSQL:(NSString*)sql
asExpectedAfterModification:(BOOL (^)())block
{
    if (object == nil
        || sql == nil) {
        return NO;
    }
    return [self checkObjects:@[ object ] andSQLs:@[ sql ] asExpectedAfterModification:block];
}

- (BOOL)checkObjects:(NSArray<TestCaseObject*>*)objects
                     andSQL:(NSString*)sql
asExpectedAfterModification:(BOOL (^)())block
{
    if (sql == nil) {
        return NO;
    }
    return [self checkObjects:objects andSQLs:@[ sql ] asExpectedAfterModification:block];
}

- (BOOL)checkObjects:(NSArray<TestCaseObject*>*)expectedObjects
                    andSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedAfterModification:(BOOL (^)())block
{
    __block ConvenientTestCaseState state = ConvenientTestCaseStateNotStarted;
    do {
        if (![expectedObjects isKindOfClass:NSArray.class]
            || expectedSQLs.count == 0
            || block == nil) {
            TESTCASE_FAILED
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (state != ConvenientTestCaseStateTestingSQL) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                NSLog(@"Failed: %@", [TestCase hint:sql expecting:sqls.firstObject]);
                state = ConvenientTestCaseStateFailed;
                TESTCASE_FAILED
            }
        }];
        if (![self.database canOpen]) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTestingSQL;
        if (!block() || sqls.count != 0) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }

        state = ConvenientTestCaseStateTestingValues;
        NSArray<TestCaseObject*>* allObjects = [self.table getObjects];
        if (![allObjects isEqualToArray:expectedObjects]) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTested;
    } while (false);
    [self.database traceSQL:nil];
    return state == ConvenientTestCaseStateTested;
}

- (BOOL)checkObject:(TestCaseObject*)object
               andSQL:(NSString*)sql
asExpectedBySelecting:(NSArray<TestCaseObject*>* (^)())block
{
    if (object == nil
        || sql == nil) {
        return NO;
    }
    return [self checkObjects:@[ object ] andSQLs:@[ sql ] asExpectedBySelecting:block];
}

- (BOOL)checkObjects:(NSArray<TestCaseObject*>*)objects
               andSQL:(NSString*)sql
asExpectedBySelecting:(NSArray<TestCaseObject*>* (^)())block
{
    if (sql == nil) {
        return NO;
    }
    return [self checkObjects:objects andSQLs:@[ sql ] asExpectedBySelecting:block];
}

- (BOOL)checkObjects:(NSArray<TestCaseObject*>*)expectedObjects
              andSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedBySelecting:(NSArray<TestCaseObject*>* (^)())block
{
    __block ConvenientTestCaseState state = ConvenientTestCaseStateNotStarted;
    do {
        if (![expectedObjects isKindOfClass:NSArray.class]
            || expectedObjects.count == 0
            || expectedSQLs.count == 0
            || block == nil) {
            TESTCASE_FAILED
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (state != ConvenientTestCaseStateTestingSQL) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                NSLog(@"Failed: %@", [TestCase hint:sql expecting:sqls.firstObject]);
                state = ConvenientTestCaseStateFailed;
                TESTCASE_FAILED
            }
        }];
        if (![self.database canOpen]) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTestingSQL;
        NSArray<TestCaseObject*>* selected = block();
        if (state != ConvenientTestCaseStateTestingSQL) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTestingValues;
        if (![selected isKindOfClass:NSArray.class]
            || ![selected isEqualToArray:expectedObjects]
            || sqls.count != 0) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTested;
    } while (false);
    [self.database traceSQL:nil];
    return state == ConvenientTestCaseStateTested;
}

- (BOOL)checkRow:(WCTOneRow*)row
               andSQL:(NSString*)sql
asExpectedBySelecting:(WCTOneRow* (^)())block
{
    if (row == nil
        || sql == nil) {
        return NO;
    }
    return [self checkRows:@[ row ]
                   andSQLs:@[ sql ]
     asExpectedBySelecting:^WCTColumnsXRows* {
         WCTOneRow* result = block();
         if (result != nil) {
             return @[ result ];
         }
         return nil;
     }];
}

- (BOOL)checkColumn:(WCTOneColumn*)column
               andSQL:(NSString*)sql
asExpectedBySelecting:(WCTOneColumn* (^)())block
{
    if (column == nil
        || sql == nil) {
        return NO;
    }
    NSMutableArray* rows = [NSMutableArray array];
    for (WCTValue* value in column) {
        [rows addObject:@[ value ]];
    }
    return [self checkRows:rows
                   andSQLs:@[ sql ]
     asExpectedBySelecting:^WCTColumnsXRows* {
         WCTOneColumn* result = block();
         if (result != nil) {
             NSMutableArray* results = [NSMutableArray array];
             for (WCTValue* value in column) {
                 [results addObject:@[ value ]];
             }
             return results;
         }
         return nil;
     }];
}

- (BOOL)checkValue:(WCTValue*)value
               andSQL:(NSString*)sql
asExpectedBySelecting:(WCTValue* (^)())block
{
    if (value == nil
        || sql == nil) {
        return NO;
    }
    return [self checkRows:@[ @[ value ] ]
                   andSQLs:@[ sql ]
     asExpectedBySelecting:^WCTColumnsXRows* {
         WCTValue* result = block();
         if (result != nil) {
             return @[ @[ value ] ];
         }
         return nil;
     }];
}

- (BOOL)checkRows:(WCTColumnsXRows*)rows
               andSQL:(NSString*)sql
asExpectedBySelecting:(WCTColumnsXRows* (^)())block
{
    if (rows == nil
        || sql == nil) {
        return NO;
    }
    return [self checkRows:rows andSQLs:@[ sql ] asExpectedBySelecting:block];
}

- (BOOL)checkRows:(WCTColumnsXRows*)expectedRows
              andSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedBySelecting:(WCTColumnsXRows* (^)())block
{
    __block ConvenientTestCaseState state = ConvenientTestCaseStateNotStarted;
    do {
        if (![expectedRows isKindOfClass:NSArray.class]
            || expectedRows.count == 0
            || expectedSQLs.count == 0
            || block == nil) {
            TESTCASE_FAILED
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (state != ConvenientTestCaseStateTestingSQL) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                NSLog(@"Failed: %@", [TestCase hint:sql expecting:sqls.firstObject]);
                state = ConvenientTestCaseStateFailed;
                TESTCASE_FAILED
            }
        }];
        if (![self.database canOpen]) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTestingSQL;
        WCTColumnsXRows* selected = block();
        if (state != ConvenientTestCaseStateTestingSQL) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTestingValues;
        if (![selected isKindOfClass:NSArray.class]
            || ![selected isEqualToArray:expectedRows]
            || sqls.count != 0) {
            state = ConvenientTestCaseStateFailed;
            TESTCASE_FAILED
            break;
        }
        state = ConvenientTestCaseStateTested;
    } while (false);
    [self.database traceSQL:nil];
    return state == ConvenientTestCaseStateTested;
}

@end
