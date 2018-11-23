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

@implementation NSNumber (TestCase)

+ (NSInteger)randomInt64
{
    static_assert(sizeof(NSInteger) == 8, "");
    NSInteger value = rand();
    value = (value << 32) | rand();
    return value;
}

+ (int)randomInt32
{
    static_assert(sizeof(int) == 4, "");
    return (int) [self randomInt64];
}

+ (uint8_t)randomUInt8
{
    return (uint8_t)[NSNumber randomInt64];
}

+ (double)randomDouble
{
    int decimals = [NSNumber randomUInt8] % 3;
    double value = [NSNumber randomInt32];
    for (int i = 0; i < decimals; ++i) {
        value /= 10;
    }
    return value;
}

+ (NSNumber *)randomNumber
{
    switch ([NSNumber randomInt32] % 3) {
    case 0:
        return [NSNumber numberWithInt:[NSNumber randomInt32]];
    case 1:
        return [NSNumber numberWithInteger:[NSNumber randomInt64]];
    case 2:
        return [NSNumber numberWithDouble:[NSNumber randomDouble]];
    }
    return nil;
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
        [randomString appendFormat:@"%c", alphanum[rand() % (sizeof(alphanum) - 1)]];
    }
    return [NSString stringWithString:randomString];
}

@end

@implementation NSData (TestCase)

+ (NSData *)randomData
{
    static_assert(sizeof(unsigned char) == 1, "");
    int length = [NSNumber randomUInt8];
    NSMutableData *data = [NSMutableData dataWithCapacity:length];
    for (NSUInteger i = 0; i < length; ++i) {
        unsigned char random = [NSNumber randomInt32] & 0xff;
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
