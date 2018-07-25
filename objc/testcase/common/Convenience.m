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

#import "Convenience.h"
#import <objc/runtime.h>

@implementation NSObject (Comparator)

+ (NSComparator)Comparator
{
    return ^NSComparisonResult(NSObject *obj1, NSObject *obj2) {
        NSUInteger hash1 = obj1.hash;
        NSUInteger hash2 = obj2.hash;
        if (hash1 < hash2) {
            return NSOrderedAscending;
        } else if (hash1 > hash2) {
            return NSOrderedDescending;
        } else {
            return NSOrderedSame;
        }
    };
}

- (NSDictionary *)dictionaryWithProperties
{
    NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
    unsigned count;

    objc_property_t *properties = class_copyPropertyList([self class], &count);

    for (int i = 0; i < count; i++) {
        NSString *key = [NSString stringWithUTF8String:property_getName(properties[i])];
        [dictionary setObject:[self valueForKey:key] forKey:key];
    }

    free(properties);

    return dictionary;
}

@end

@implementation NSArray (Reverse)

- (NSArray *)sorted
{
    return [self sortedArrayUsingComparator:NSObject.Comparator];
}

- (NSArray *)reversed
{
    NSMutableArray *reversedArray = [[NSMutableArray alloc] initWithCapacity:self.count];
    NSEnumerator *reversedEnum = self.reverseObjectEnumerator;
    NSObject *object = nil;
    while (object = [reversedEnum nextObject]) {
        [reversedArray addObject:object];
    }
    return reversedArray;
}

- (BOOL)isEqualToObjects:(NSArray *)objects
          withComparator:(BOOL (^)(id, id))comparator
{
    if (self.count != objects.count) {
        return NO;
    }
    for (int i = 0; i < self.count; ++i) {
        id lhs = [self objectAtIndex:i];
        id rhs = [objects objectAtIndex:i];
        if (!comparator(lhs, rhs)) {
            return NO;
        }
    }
    return YES;
}

@end

@implementation NSMutableArray (Reverse)

- (NSMutableArray *)sorted
{
    return [NSMutableArray arrayWithArray:[self sortedArrayUsingComparator:NSObject.Comparator]];
}

- (NSMutableArray *)reversed
{
    NSMutableArray *reversedArray = [[NSMutableArray alloc] initWithCapacity:self.count];
    NSEnumerator *reversedEnum = self.reverseObjectEnumerator;
    NSObject *object = nil;
    while (object = [reversedEnum nextObject]) {
        [reversedArray addObject:object];
    }
    return reversedArray;
}
@end

@implementation NSData (Random)

+ (NSData *)randomData
{
    int length = [NSNumber randomUInt8];
    return [self randomDataWithLength:length];
}

+ (NSData *)randomDataWithLength:(NSUInteger)length
{
    static_assert(sizeof(unsigned char) == 1, "");
    NSMutableData *data = [NSMutableData dataWithCapacity:length];
    for (NSUInteger i = 0; i < length; ++i) {
        unsigned char random = rand() & 0xff;
        [data appendBytes:&random length:sizeof(unsigned char)];
    }
    return [NSData dataWithData:data];
}

+ (NSData *)randomDataOtherThan:(NSData *)other
{
    NSData *data;
    do {
        data = [self randomData];
    } while ([data isEqualToData:other]);
    return data;
}

@end

@implementation NSString (Random)

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
        [randomString appendFormat:@"%c", alphanum[rand() % (sizeof(alphanum) - 1)]];
    }
    return [NSString stringWithString:randomString];
}

+ (BOOL)isNullableString:(NSString *)lhs equalTo:(NSString *)rhs
{
    if (lhs == nil) {
        return rhs == nil;
    }
    return [rhs isEqualToString:lhs];
}

@end

@implementation NSNumber (Random)

static_assert(sizeof(rand()) == sizeof(int32_t), "");

+ (NSNumber *)randomNumber
{
    if ([NSNumber randomBool]) {
        return [NSNumber numberWithLongLong:[NSNumber randomInt64]];
    }
    return [NSNumber numberWithDouble:[NSNumber randomDouble]];
}

+ (int8_t)randomInt8
{
    return [self randomInt32];
}

+ (int16_t)randomInt16
{
    return [self randomInt32];
}

+ (int32_t)randomInt32
{
    return rand();
}

+ (int64_t)randomInt64
{
    int64_t value = rand();
    value = (value << 32) | rand();
    return value;
}

+ (unsigned int)randomUInt
{
    return (unsigned int) [NSNumber randomInt64];
}

+ (uint8_t)randomUInt8
{
    return [NSNumber randomInt64];
}

+ (double)randomDouble
{
    return rand() / (double) (RAND_MAX) *DBL_MAX;
}

+ (BOOL)randomBool
{
    return rand() % 2;
}

@end
