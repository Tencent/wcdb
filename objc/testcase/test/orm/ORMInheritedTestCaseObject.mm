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

#import "ORMInheritedTestCaseObject.h"
#import "ORMInheritedTestCaseObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation ORMInheritedTestCaseObject

WCDB_IMPLEMENTATION(ORMInheritedTestCaseObject)
WCDB_SYNTHESIZE(ORMInheritedTestCaseObject, variable1)
WCDB_SYNTHESIZE(ORMInheritedTestCaseObject, variable2)
WCDB_SYNTHESIZE(ORMInheritedTestCaseObject, variable3)
WCDB_SYNTHESIZE(ORMInheritedTestCaseObject, variable4)
WCDB_SYNTHESIZE(ORMInheritedTestCaseObject, variable5)

+ (instancetype)objectWithId:(int)i
{
    ORMInheritedTestCaseObject *object = [[ORMInheritedTestCaseObject alloc] init];
    object.variable1 = i;

    object.variable2 = [NSString randomString];
    object.variable3 = (float) rand() / (float) (RAND_MAX / 100);
    object.variable4 = [NSString randomString];
    object.variable5 = (float) rand() / (float) (RAND_MAX / 100);

    return object;
}

- (BOOL)isEqualToObject:(ORMInheritedTestCaseObject *)object
{
    if (![object isKindOfClass:self.class]) {
        return NO;
    }
    return self.variable1 == object.variable1 && [self.variable2 isEqualToString:object.variable2] && self.variable3 == object.variable3 && [self.variable4 isEqualToString:object.variable4] && self.variable5 == object.variable5;
}

@end
