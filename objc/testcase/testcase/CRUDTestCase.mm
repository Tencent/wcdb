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

#import <TestCase/CRUDTestCase.h>
#import <TestCase/TestCaseAssertion.h>

@implementation CRUDTestCase

- (void)setUp
{
    [super setUp];

    _object1 = [[TestCaseObject alloc] init];
    _object1.identifier = 1;
    _object1.content = self.random.string;

    _object2 = [[TestCaseObject alloc] init];
    _object2.identifier = 2;
    _object2.content = self.random.string;

    _objects = @[ _object1, _object2 ];

    TestCaseAssertTrue([self.table insertObjects:_objects]);

    [self.database close];
}

- (BOOL)checkObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
            andInsertSQL:(NSString*)insertSQL
               withCount:(int)count
asExpectedAfterInsertion:(BOOL (^)())block
{
    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    if (count > 1) {
        [sqls addObject:@"BEGIN IMMEDIATE"];
    }
    for (int i = 0; i < count; ++i) {
        [sqls addObject:insertSQL];
    }
    if (count > 1) {
        [sqls addObject:@"COMMIT"];
    }
    return [self checkObjects:objects andSQLs:sqls asExpectedAfterModification:block];
}

- (BOOL)checkObject:(NSObject<WCTTableCoding>*)object
                     andSQL:(NSString*)sql
asExpectedAfterModification:(BOOL (^)())block
{
    if (object == nil
        || sql == nil) {
        TestCaseFailure();
        return NO;
    }
    return [self checkObjects:@[ object ] andSQLs:@[ sql ] asExpectedAfterModification:block];
}

- (BOOL)checkObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
                     andSQL:(NSString*)sql
asExpectedAfterModification:(BOOL (^)())block
{
    if (sql == nil) {
        TestCaseFailure();
        return NO;
    }
    return [self checkObjects:objects andSQLs:@[ sql ] asExpectedAfterModification:block];
}

- (BOOL)checkObjects:(NSArray<NSObject<WCTTableCoding>*>*)expectedObjects
                    andSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedAfterModification:(BOOL (^)())block
{
    if (![self checkAllSQLs:expectedSQLs
        asExpectedInOperation:^BOOL {
            return block();
        }]) {
        return NO;
    }
    NSArray<NSObject<WCTTableCoding>*>* allObjects = [self.table getObjects];
    if (![allObjects isEqualToArray:expectedObjects]) {
        TestCaseFailure();
        return NO;
    }
    return YES;
}

- (BOOL)checkObject:(NSObject<WCTTableCoding>*)object
               andSQL:(NSString*)sql
asExpectedBySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block
{
    if (object == nil
        || sql == nil) {
        TestCaseFailure();
        return NO;
    }
    return [self checkObjects:@[ object ] andSQLs:@[ sql ] asExpectedBySelecting:block];
}

- (BOOL)checkObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
               andSQL:(NSString*)sql
asExpectedBySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block
{
    if (sql == nil) {
        TestCaseFailure();
        return NO;
    }
    return [self checkObjects:objects andSQLs:@[ sql ] asExpectedBySelecting:block];
}

- (BOOL)checkObjects:(NSArray<NSObject<WCTTableCoding>*>*)expectedObjects
              andSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedBySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block
{
    __block NSArray<NSObject<WCTTableCoding>*>* selected;
    if (![self checkAllSQLs:expectedSQLs
        asExpectedInOperation:^BOOL {
            selected = block();
            return selected != nil;
        }]) {
        return NO;
    }
    if (![selected isKindOfClass:NSArray.class]
        || ![selected isEqualToArray:expectedObjects]) {
        TestCaseFailure();
        return NO;
    }
    return YES;
}

- (BOOL)checkRow:(WCTOneRow*)row
               andSQL:(NSString*)sql
asExpectedBySelecting:(WCTOneRow* (^)())block
{
    if (row == nil
        || sql == nil) {
        TestCaseFailure();
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
        TestCaseFailure();
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
        TestCaseFailure();
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
        TestCaseFailure();
        return NO;
    }
    return [self checkRows:rows andSQLs:@[ sql ] asExpectedBySelecting:block];
}

- (BOOL)checkRows:(WCTColumnsXRows*)expectedRows
              andSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedBySelecting:(WCTColumnsXRows* (^)())block
{
    __block WCTColumnsXRows* selected;
    if (![self checkAllSQLs:expectedSQLs
        asExpectedInOperation:^BOOL {
            selected = block();
            return selected != nil;
        }]) {
        return NO;
    }
    if (![selected isKindOfClass:NSArray.class]
        || ![selected isEqualToArray:expectedRows]) {
        TestCaseFailure();
        return NO;
    }
    return YES;
}

@end
