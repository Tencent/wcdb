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

#import "sample_convenient_main.h"
#import "WCTSampleConvenient+WCTTableCoding.h"
#import "WCTSampleConvenient.h"

void sample_convenient_main(NSString *baseDirectory)
{
    NSLog(@"Sample-convenient Begin");
    NSString *className = NSStringFromClass(WCTSampleConvenient.class);
    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
    NSString *tableName = className;
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];
    BOOL ret = [database createTableAndIndexesOfName:tableName withClass:WCTSampleConvenient.class];
    assert(ret);

    //Insert one object
    {
        WCTSampleConvenient *object = [[WCTSampleConvenient alloc] init];
        object.intValue = 1;
        object.stringValue = @"Insert one object";
        [database insertObject:object
                          into:tableName];
    }
    //Insert objects
    {
        NSMutableArray *objects = [[NSMutableArray alloc] init];
        WCTSampleConvenient *object1 = [[WCTSampleConvenient alloc] init];
        object1.intValue = 2;
        object1.stringValue = @"Insert objects";
        [objects addObject:object1];
        WCTSampleConvenient *object2 = [[WCTSampleConvenient alloc] init];
        object2.intValue = 3;
        object2.stringValue = @"Insert objects";
        [objects addObject:object2];
        [database insertObjects:objects
                           into:tableName];
    }
    //Insert or replace one object
    {
        WCTSampleConvenient *object = [[WCTSampleConvenient alloc] init];
        object.intValue = 1;
        object.stringValue = @"Insert or replace one object";
        [database insertOrReplaceObject:object
                                   into:tableName];
    }
    //Insert or replace objects
    {
        NSMutableArray *objects = [[NSMutableArray alloc] init];
        WCTSampleConvenient *object1 = [[WCTSampleConvenient alloc] init];
        object1.intValue = 2;
        object1.stringValue = @"Insert or replace objects";
        [objects addObject:object1];
        WCTSampleConvenient *object2 = [[WCTSampleConvenient alloc] init];
        object2.intValue = 3;
        object2.stringValue = @"Insert or replace objects";
        [objects addObject:object2];
        [database insertOrReplaceObjects:objects
                                    into:tableName];
    }
    //Insert auto increment
    {
        WCTSampleConvenient *object = [[WCTSampleConvenient alloc] init];
        object.isAutoIncrement = YES;
        object.stringValue = @"Insert auto increment";
        [database insertObject:object
                          into:tableName];
    }

    //Update by object
    {
        WCTSampleConvenient *object = [[WCTSampleConvenient alloc] init];
        object.stringValue = @"Update by object";
        [database updateAllRowsInTable:tableName
                          onProperties:WCTSampleConvenient.stringValue
                            withObject:object];
    }
    //Update by value
    {
        NSArray *row = [NSArray arrayWithObject:@"Update by value"];
        [database updateAllRowsInTable:tableName
                          onProperties:WCTSampleConvenient.stringValue
                               withRow:row];
    }
    //Update with condition/order/offset/limit
    {
        WCTSampleConvenient *object = [[WCTSampleConvenient alloc] init];
        object.stringValue = @"Update with condition/order/offset/limit";
        [database updateRowsInTable:tableName
                       onProperties:WCTSampleConvenient.stringValue
                         withObject:object
                              where:WCTSampleConvenient.intValue > 0];
    }

    //Select One Object
    {
        WCTSampleConvenient *object = [database getOneObjectOfClass:WCTSampleConvenient.class
                                                          fromTable:tableName];
    }
    //Select Objects
    {
        NSArray<WCTSampleConvenient *> *objects = [database getAllObjectsOfClass:WCTSampleConvenient.class
                                                                       fromTable:tableName];
    }
    //Select Objects with condition/order/offset/limit
    {
        NSArray<WCTSampleConvenient *> *objects = [database getObjectsOfClass:WCTSampleConvenient.class
                                                                    fromTable:tableName
                                                                      orderBy:WCTSampleConvenient.intValue.order(WCTOrderedAscending)
                                                                        limit:1
                                                                       offset:2];
    }
    //Select Part of Objects
    {
        NSArray<WCTSampleConvenient *> *objects = [database getAllObjectsOnResults:WCTSampleConvenient.stringValue
                                                                         fromTable:tableName];
    }
    //Select column
    {
        WCTOneColumn *objects = [database getOneColumnOnResult:WCTSampleConvenient.stringValue
                                                     fromTable:tableName];
        for (NSString *string : objects) {
            //do sth
        }
    }
    //Select row
    {
        WCTOneRow *row = [database getOneRowOnResults:{
                                                          WCTSampleConvenient.intValue,
                                                          WCTSampleConvenient.stringValue}
                                            fromTable:tableName];
        NSNumber *intValue = (NSNumber *) row[0];
        NSString *stringValue = (NSString *) row[1];
    }
    //Select one value
    {
        NSNumber *count = [database getOneValueOnResult:WCTSampleConvenient.AnyProperty.count()
                                              fromTable:tableName];
    }
    //Select aggregation
    {
        WCTOneRow *row = [database getOneRowOnResults:{
                                                          WCTSampleConvenient.intValue.avg(),
                                                          WCTSampleConvenient.stringValue.count(),
                                                      }
                                            fromTable:tableName];
    }
    //Select distinct aggregation
    {
        NSArray *objects = [database getAllObjectsOnResults:WCTSampleConvenient.stringValue.count(true)
                                                  fromTable:tableName];
    }
    //Select distinct result
    {
        NSNumber *distinctCount = [database getOneDistinctValueOnResult:WCTSampleConvenient.intValue fromTable:tableName];
    }

    //Delete
    {
        BOOL ret = [database deleteAllObjectsFromTable:tableName];
    }
    //Delete with condition/order/offset/limit
    {
        BOOL ret = [database deleteObjectsFromTable:tableName
                                              where:WCTSampleConvenient.intValue.in({1, 2, 3})];
    }
    //Delete with condition/order/offset/limit
    {
        BOOL ret = [database deleteObjectsFromTable:tableName
                                              where:WCTSampleConvenient.intValue.in(@[ @(1), @(2), @(3) ])];
    }
    NSLog(@"Sample-convenient End");
}
