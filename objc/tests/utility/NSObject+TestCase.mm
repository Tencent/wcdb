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

#import "NSObject+TestCase.h"

@implementation NSObject (TestCase)

+ (BOOL)isObject:(NSObject *)left nilEqualToObject:(NSObject *)right
{
    return (left == nil && right == nil) || [left isEqual:right];
}

@end

@implementation NSNumber (TestCase)

+ (int64_t)randomInt64
{
    return (int64_t)[self randomUInt64];
}

+ (uint64_t)randomUInt64
{
    uint64_t value = arc4random();
    value = (value << 32) | arc4random();
    return value;
}

+ (int32_t)randomInt32
{
    static_assert(sizeof(int) == 4, "");
    return (int32_t)[self randomUInt64];
}

+ (uint32_t)randomUInt32
{
    return (uint32_t)[self randomUInt64];
}

+ (uint8_t)randomUInt8
{
    return (uint8_t)[NSNumber randomUInt64];
}

+ (double)randomDouble
{
#define ARC4RANDOM_MAX 0x100000000
    return [NSString stringWithFormat:@"%g", ((double) arc4random() / ARC4RANDOM_MAX) * [NSNumber randomInt32]].numberValue.doubleValue;
}

+ (NSNumber *)randomNumber
{
    switch ([NSNumber randomUInt8] % 3) {
    case 0:
        return [NSNumber numberWithInt:[NSNumber randomInt32]];
    case 1:
        return [NSNumber numberWithLongLong:[NSNumber randomInt64]];
    case 2:
        return [NSNumber numberWithDouble:[NSNumber randomDouble]];
    }
    return nil;
}

+ (float)random_0_1
{
    return (float) rand() / RAND_MAX;
}

+ (BOOL)randomBool
{
    return [NSNumber randomUInt64] % 2;
}

@end

@implementation NSString (TestCase)

+ (NSString *)randomString
{
    int length;
    do {
        length = [NSNumber randomUInt8];
    } while (length < 8);
    static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    NSMutableString *randomString = [NSMutableString string];
    for (int i = 0; i < length; ++i) {
        [randomString appendFormat:@"%c", alphanum[arc4random() % (sizeof(alphanum) - 1)]];
    }
    return [NSString stringWithString:randomString];
}

@end

@implementation NSData (TestCase)

+ (NSData *)randomData
{
    return [NSData randomDataWithLength:[NSNumber randomUInt8]];
}

+ (NSData *)randomDataWithLength:(NSInteger)length
{
    static_assert(sizeof(unsigned char) == 1, "");
    NSMutableData *data = [NSMutableData dataWithCapacity:length];
    for (NSUInteger i = 0; i < length; ++i) {
        unsigned char random = [NSNumber randomUInt64] & 0xff;
        [data appendBytes:&random length:sizeof(unsigned char)];
    }
    return [NSData dataWithData:data];
}

+ (NSData *)randomDataOtherThan:(NSData *)other
{
    NSData *data;
    do {
        data = [NSData randomData];
    } while ([other isEqualToData:data]);
    return data;
}

@end

@implementation NSArray (TestCase)

- (NSArray *)reversedArray
{
    return self.reverseObjectEnumerator.allObjects;
}

@end

@implementation WCTPerformanceFootprint (TestCase)

- (BOOL)isEqual:(NSObject *)object
{
    if (object.class != self.class) {
        return NO;
    }
    WCTPerformanceFootprint *other = (WCTPerformanceFootprint *) object;
    if (self.frequency != other.frequency) {
        return NO;
    }
    if (self.sql != nil) {
        return [other.sql isEqualToString:self.sql];
    } else {
        return other.sql == nil;
    }
}

@end
