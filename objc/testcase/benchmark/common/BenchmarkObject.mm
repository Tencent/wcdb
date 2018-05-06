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

#import "BenchmarkObject.h"
#import "BenchmarkObject+WCTTableCoding.h"
#import "Convenience.h"
#import <WCDB/WCDB.h>

@implementation BenchmarkObject

WCDB_IMPLEMENTATION(BenchmarkObject)
WCDB_SYNTHESIZE(BenchmarkObject, key)
WCDB_SYNTHESIZE(BenchmarkObject, value)

- (instancetype)initWithKey:(int64_t)key andValue:(NSData *)value
{
    if (self = [super init]) {
        _key = key;
        _value = value;
    }
    return self;
}

+ (NSString *)name
{
    return NSStringFromClass(BenchmarkObject.class);
}

- (BOOL)isEqualToBenchmarkObject:(BenchmarkObject *)other
{
    if (other.class != BenchmarkObject.class) {
        return NO;
    }
    if (other.key != self.key) {
        return NO;
    }
    if (other.value == nil && self.value == nil) {
        return YES;
    }
    if (other.value != nil && self.value != nil) {
        return [other.value isEqualToData:self.value];
    }
    return NO;
}

@end

@implementation NSArray (BenchmarkObject)

- (BOOL)isEqualToBenchmarkObjects:(NSArray<BenchmarkObject *> *)objects
{
    return [self isEqualToObjects:objects
                   withComparator:^BOOL(BenchmarkObject *lhs, BenchmarkObject *rhs) {
                     return [lhs isEqualToBenchmarkObject:rhs];
                   }];
}

@end
