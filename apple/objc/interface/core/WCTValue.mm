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

#import <WCDB/Assertion.hpp>
#import <WCDB/Core.h>
#import <WCDB/WCTValue.h>

@implementation NSNumber (WCTValue)

- (NSData *)dataValue
{
    return [self.stringValue dataUsingEncoding:NSUTF8StringEncoding];
}

- (NSNumber *)numberValue
{
    return self;
}

@end

@implementation NSData (WCTValue)

- (NSData *)dataValue
{
    return self;
}

- (NSString *)stringValue
{
    return [[NSString alloc] initWithData:self encoding:NSUTF8StringEncoding];
}

- (NSNumber *)numberValue
{
    return self.stringValue.numberValue;
}

@end

@implementation NSString (WCTValue)

- (NSData *)dataValue
{
    return [self dataUsingEncoding:NSUTF8StringEncoding];
}

- (NSString *)stringValue
{
    return self;
}

- (NSNumber *)numberValue
{
    NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
    return [formatter numberFromString:self];
}

@end

@implementation NSNull (WCTValue)

- (NSData *)dataValue
{
    return nil;
}

- (NSString *)stringValue
{
    return nil;
}

- (NSNumber *)numberValue
{
    return nil;
}

@end
