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
    return [self randomDataWithLength:256];
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
    int length = rand() % 256;
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
