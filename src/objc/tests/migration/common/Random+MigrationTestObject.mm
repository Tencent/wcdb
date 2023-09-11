//
// Created by 陈秋文 on 2023/7/23.
//

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

#import "MigrationTestSourceObject.h"
#import "Random+MigrationTestObject.h"
#import <Foundation/Foundation.h>

@implementation Random (MigrationTestObject)

- (NSObject<MigrationTestObject>*)autoIncrementMigrationObjectWithClass:(Class<MigrationTestObject>)objectClass
{
    NSObject<MigrationTestObject>* object = [self migrationObjectWithClass:objectClass
                                                             andIdentifier:Random.shared.int32
                                                            withoutContent:NO];
    object.isAutoIncrement = YES;
    return object;
}

- (NSObject<MigrationTestObject>*)migrationObjectWithClass:(Class<MigrationTestObject>)objectClass
                                             andIdentifier:(int)identifier
{
    return [self migrationObjectWithClass:objectClass andIdentifier:identifier withoutContent:NO];
}

- (NSObject<MigrationTestObject>*)migrationObjectWithClass:(Class<MigrationTestObject>)objectClass
                                             andIdentifier:(int)identifier
                                            withoutContent:(bool)noContent
{
    NSObject<MigrationTestObject>* object = [[(Class) objectClass alloc] init];
    object.identifier = identifier;
    if ([object conformsToProtocol:@protocol(MigrationTestSourceObject)]) {
        id<MigrationTestSourceObject> sourceObject = (id<MigrationTestSourceObject>) object;
        sourceObject.classification = (MigrationClassification) (self.uint8 % 3);
    }
    if (!noContent) {
        object.content = self.string;
    }
    return object;
}

- (NSArray<NSObject<MigrationTestObject>*>*)migrationObjectsWithClass:(Class<MigrationTestObject>)objectClass
                                                             andCount:(int)count
                                               startingFromIdentifier:(int)identifier
                                                       withoutContent:(bool)noContent
{
    NSMutableArray<NSObject<MigrationTestObject>*>* objects = [NSMutableArray arrayWithCapacity:count];
    for (int i = identifier; i < identifier + count; ++i) {
        [objects addObject:[self migrationObjectWithClass:objectClass
                                            andIdentifier:i
                                           withoutContent:noContent]];
    }
    return objects;
}

@end
