//
// Created by sanhuazhang on 2019/05/02
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

#import "Random+MigrationObject.h"

@implementation Random (MigrationObject)

- (MigrationObject*)autoIncrementMigrationObjectWithIdentifier:(int)identifier
{
    MigrationObject* object = [MigrationObject objectWithIdentifier:identifier andContent:self.string];
    object.isAutoIncrement = YES;
    return object;
}

- (MigrationObject*)migrationObjectWithIdentifier:(int)identifier
{
    return [self migrationObjectWithIdentifier:identifier withoutContent:NO];
}

- (MigrationObject*)migrationObjectWithIdentifier:(int)identifier withoutContent:(bool)noContent
{
    if(!noContent){
        return [MigrationObject objectWithIdentifier:identifier andContent:self.string];
    }else{
        return [MigrationObject objectWithIdentifier:identifier andContent:nil];
    }
}

- (NSArray<MigrationObject*>*)migrationObjectsWithCount:(NSUInteger)count startingFromIdentifier:(int)identifier withoutContent:(bool)noContent;
{
    NSMutableArray<MigrationObject*>* objects = [NSMutableArray arrayWithCapacity:count];
    for (NSUInteger i = identifier; i < identifier + count; ++i) {
        [objects addObject:[self migrationObjectWithIdentifier:(int) i withoutContent:noContent]];
    }
    return objects;
}

@end
