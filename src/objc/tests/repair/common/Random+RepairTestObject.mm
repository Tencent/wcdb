//
// Created by 陈秋文 on 2023/9/15.
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

#import "Random+RepairTestObject.h"
#import <Foundation/Foundation.h>

@implementation Random (RepairTestObject)

- (NSObject<RepairTestObject>*)repairObjectWithClass:(Class<RepairTestObject>)objectClass
                                       andIdentifier:(int)identifier
{
    NSObject<RepairTestObject>* object = [[(Class) objectClass alloc] init];
    Random* random = Random.shared;
    object.identifier = identifier;
    object.doubleValue = random.double_;
    object.textValue = random.string;
    object.blobValue = random.data;
    return object;
}

- (NSArray<NSObject<RepairTestObject>*>*)repairObjectsWithClass:(Class<RepairTestObject>)objectClass
                                                       andCount:(int)count
                                         startingFromIdentifier:(int)identifier
{
    NSMutableArray<NSObject<RepairTestObject>*>* objects = [NSMutableArray arrayWithCapacity:count];
    for (int i = identifier; i < identifier + count; ++i) {
        [objects addObject:[self repairObjectWithClass:objectClass
                                         andIdentifier:i]];
    }
    return objects;
}

@end
