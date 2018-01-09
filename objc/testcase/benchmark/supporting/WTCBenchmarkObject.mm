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

#import "WTCBenchmarkObject.h"
#import "WTCBenchmarkObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@interface WTCBenchmarkObject ()
@property(nonatomic, assign) int64_t key;
@property(nonatomic, retain) NSData *value;
@end

@implementation WTCBenchmarkObject

WCDB_IMPLEMENTATION(WTCBenchmarkObject)
WCDB_SYNTHESIZE(WTCBenchmarkObject, key)
WCDB_SYNTHESIZE(WTCBenchmarkObject, value)

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
    return NSStringFromClass(WTCBenchmarkObject.class);
}

@end
