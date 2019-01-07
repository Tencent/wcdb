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

@implementation CRUDTestCase {
    TestCaseObject* _object1;
    TestCaseObject* _object2;
    NSArray<TestCaseObject*>* _objects;
}

- (TestCaseObject*)object1
{
    if (!_object1) {
        TestCaseObject* object = [[TestCaseObject alloc] init];
        object.identifier = 1;
        object.content = self.random.string;
        _object1 = object;
    }
    return _object1;
}

- (TestCaseObject*)object2
{
    if (!_object2) {
        TestCaseObject* object = [[TestCaseObject alloc] init];
        object.identifier = 2;
        object.content = self.random.string;
        _object2 = object;
    }
    return _object2;
}

- (NSArray<TestCaseObject*>*)objects
{
    if (!_objects) {
        _objects = @[ self.object1, self.object2 ];
    }
    return _objects;
}

- (void)insertPresetObjects
{
    TestCaseAssertTrue([self.table insertObjects:self.objects]);
}

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
            andNumber:(int)numberOfInsertSQLs
         ofInsertSQLs:(NSString*)insertSQL
       afterInsertion:(BOOL (^)())block
{
    TestCaseAssertTrue(numberOfInsertSQLs > 0);
    TestCaseAssertTrue(insertSQL != nil);
    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    if (numberOfInsertSQLs > 1) {
        [sqls addObject:@"BEGIN IMMEDIATE"];
    }
    for (int i = 0; i < numberOfInsertSQLs; ++i) {
        [sqls addObject:insertSQL];
    }
    if (numberOfInsertSQLs > 1) {
        [sqls addObject:@"COMMIT"];
    }
    [self doTestObjects:objects andSQLs:sqls afterModification:block];
}

- (void)doTestObject:(NSObject<WCTTableCoding>*)object
              andSQL:(NSString*)sql
   afterModification:(BOOL (^)())block
{
    TestCaseAssertTrue(object != nil);
    TestCaseAssertTrue(sql != nil);
    [self doTestObjects:@[ object ] andSQLs:@[ sql ] afterModification:block];
}

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
               andSQL:(NSString*)sql
    afterModification:(BOOL (^)())block
{
    TestCaseAssertTrue(sql != nil);
    [self doTestObjects:objects andSQLs:@[ sql ] afterModification:block];
}

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)expectedObjects
              andSQLs:(NSArray<NSString*>*)expectedSQLs
    afterModification:(BOOL (^)())block
{
    [self doTestSQLs:expectedSQLs inOperation:block];
    NSArray<NSObject<WCTTableCoding>*>* allObjects = [self.table getObjects];
    TestCaseAssertTrue([allObjects isEqualToArray:expectedObjects]);
}

- (void)doTestObject:(NSObject<WCTTableCoding>*)object
              andSQL:(NSString*)sql
         bySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block
{
    TestCaseAssertTrue(object != nil);
    TestCaseAssertTrue(sql != nil);
    [self doTestObjects:@[ object ] andSQLs:@[ sql ] bySelecting:block];
}

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
               andSQL:(NSString*)sql
          bySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block
{
    TestCaseAssertTrue(sql != nil);
    [self doTestObjects:objects andSQLs:@[ sql ] bySelecting:block];
}

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)expectedObjects
              andSQLs:(NSArray<NSString*>*)expectedSQLs
          bySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block
{
    __block NSArray<NSObject<WCTTableCoding>*>* selected;
    [self doTestSQLs:expectedSQLs
         inOperation:^BOOL {
             selected = block();
             return selected != nil;
         }];
    TestCaseAssertTrue([selected isKindOfClass:NSArray.class]);
    TestCaseAssertTrue([selected isEqualToArray:expectedObjects]);
}

- (void)doTestRow:(WCTOneRow*)row
           andSQL:(NSString*)sql
      bySelecting:(WCTOneRow* (^)())block
{
    TestCaseAssertTrue(row != nil);
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:@[ row ]
             andSQLs:@[ sql ]
         bySelecting:^WCTColumnsXRows* {
             WCTOneRow* result = block();
             if (result != nil) {
                 return @[ result ];
             }
             return nil;
         }];
}

- (void)doTestColumn:(WCTOneColumn*)column
              andSQL:(NSString*)sql
         bySelecting:(WCTOneColumn* (^)())block
{
    TestCaseAssertTrue(sql != nil);
    TestCaseAssertTrue(column != nil);
    NSMutableArray* rows = [NSMutableArray array];
    for (WCTValue* value in column) {
        [rows addObject:@[ value ]];
    }
    [self doTestRows:rows
             andSQLs:@[ sql ]
         bySelecting:^WCTColumnsXRows* {
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

- (void)doTestValue:(WCTValue*)value
             andSQL:(NSString*)sql
        bySelecting:(WCTValue* (^)())block
{
    TestCaseAssertTrue(value != nil);
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:@[ @[ value ] ]
             andSQLs:@[ sql ]
         bySelecting:^WCTColumnsXRows* {
             WCTValue* result = block();
             if (result != nil) {
                 return @[ @[ value ] ];
             }
             return nil;
         }];
}

- (void)doTestRows:(WCTColumnsXRows*)rows
            andSQL:(NSString*)sql
       bySelecting:(WCTColumnsXRows* (^)())block
{
    TestCaseAssertTrue(rows != nil);
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:rows andSQLs:@[ sql ] bySelecting:block];
}

- (void)doTestRows:(WCTColumnsXRows*)expectedRows
           andSQLs:(NSArray<NSString*>*)expectedSQLs
       bySelecting:(WCTColumnsXRows* (^)())block
{
    __block WCTColumnsXRows* selected;
    [self doTestSQLs:expectedSQLs
         inOperation:^BOOL {
             selected = block();
             return selected != nil;
         }];
    TestCaseAssertTrue([selected isKindOfClass:NSArray.class]);
    TestCaseAssertTrue([selected isEqualToArray:expectedRows]);
}

@end
