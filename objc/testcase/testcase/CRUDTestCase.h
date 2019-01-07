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

#import <TestCase/TableTestCase.h>
#import <TestCase/TestCaseObject+WCTTableCoding.h>
#import <TestCase/TestCaseObject.h>

@interface CRUDTestCase : TableTestCase

@property (nonatomic, readonly) TestCaseObject* object1;
@property (nonatomic, readonly) TestCaseObject* object2;
@property (nonatomic, readonly) NSArray<TestCaseObject*>* objects;
- (void)insertPresetObjects;

- (void)doTestObject:(NSObject<WCTTableCoding>*)object
              andSQL:(NSString*)sql
   afterModification:(BOOL (^)())block;

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
               andSQL:(NSString*)sql
    afterModification:(BOOL (^)())block;

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
              andSQLs:(NSArray<NSString*>*)sqls
    afterModification:(BOOL (^)())block;

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
            andNumber:(int)numberOfInsertSQLs
         ofInsertSQLs:(NSString*)insertSQL
       afterInsertion:(BOOL (^)())block;

- (void)doTestObject:(NSObject<WCTTableCoding>*)object
              andSQL:(NSString*)sql
         bySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block;

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)objects
               andSQL:(NSString*)sql
          bySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block;

- (void)doTestObjects:(NSArray<NSObject<WCTTableCoding>*>*)expectedObjects
              andSQLs:(NSArray<NSString*>*)expectedSQLs
          bySelecting:(NSArray<NSObject<WCTTableCoding>*>* (^)())block;

- (void)doTestRow:(WCTOneRow*)row
           andSQL:(NSString*)sql
      bySelecting:(WCTOneRow* (^)())block;

- (void)doTestColumn:(WCTOneColumn*)column
              andSQL:(NSString*)sql
         bySelecting:(WCTOneColumn* (^)())block;

- (void)doTestValue:(WCTValue*)value
             andSQL:(NSString*)sql
        bySelecting:(WCTValue* (^)())block;

- (void)doTestRows:(WCTColumnsXRows*)rows
            andSQL:(NSString*)sql
       bySelecting:(WCTColumnsXRows* (^)())block;

- (void)doTestRows:(WCTColumnsXRows*)rows
           andSQLs:(NSArray<NSString*>*)sqls
       bySelecting:(WCTColumnsXRows* (^)())block;

@end
