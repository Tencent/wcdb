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
#import <WCDB/Interface.h>
#import <WCDB/WCTValue+Private.h>

@implementation WCTValueForwarder {
    NSString *_string;
    NSNumber *_number;
    NSData *_data;
}

+ (WCTValueForwarder *)forwarder
{
    static WCTValueForwarder *s_value = [[WCTValueForwarder alloc] init];
    return s_value;
}

- (instancetype)init
{
    if (self = [super init]) {
        _string = [[NSString alloc] init];
        _number = @0;
        _data = [[NSData alloc] init];
    }
    return self;
}

- (BOOL)respondsToForwardSelector:(SEL)aSelector
{
    return [_string respondsToSelector:aSelector] || [_number respondsToSelector:aSelector] || [_data respondsToSelector:aSelector];
}

- (void)forwardInvocation:(NSInvocation *)invocation
{
    SEL aSelector = invocation.selector;
    if ([_string respondsToSelector:aSelector]) {
        [invocation invokeWithTarget:_string];
    } else if ([_number respondsToSelector:aSelector]) {
        [invocation invokeWithTarget:_number];
    } else if ([_data respondsToSelector:aSelector]) {
        [invocation invokeWithTarget:_data];
    } else {
        [super forwardInvocation:invocation];
    }
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    if ([_string respondsToSelector:aSelector]) {
        return [_string methodSignatureForSelector:aSelector];
    } else if ([_number respondsToSelector:aSelector]) {
        return [_number methodSignatureForSelector:aSelector];
    } else if ([_data respondsToSelector:aSelector]) {
        return [_data methodSignatureForSelector:aSelector];
    } else {
        return [super methodSignatureForSelector:aSelector];
    }
}

@end

@implementation NSNumber (WCTValue)

- (WCTColumnType)valueType
{
    if (CFNumberIsFloatType((CFNumberRef) self)) {
        return WCTColumnTypeFloat;
    } else {
        if (CFNumberGetByteSize((CFNumberRef) self) <= 4) {
            return WCTColumnTypeInteger32;
        } else {
            return WCTColumnTypeInteger64;
        }
    }
}

- (NSData *)dataValue
{
    return [self.stringValue dataUsingEncoding:NSASCIIStringEncoding];
}

- (NSNumber *)numberValue
{
    return self;
}

@end

@implementation NSData (WCTValue)

- (WCTColumnType)valueType
{
    return WCTColumnTypeData;
}

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
    NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
    return [formatter numberFromString:self.stringValue];
}

@end

@implementation NSString (WCTValue)

- (WCTColumnType)valueType
{
    return WCTColumnTypeString;
}

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

- (WCTColumnType)valueType
{
    return WCTColumnTypeNil;
}

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

- (void)forwardInvocation:(NSInvocation *)invocation
{
    SEL aSelector = invocation.selector;
    WCTValueForwarder *forwareder = [WCTValueForwarder forwarder];
    if ([forwareder respondsToForwardSelector:aSelector]) {
        [invocation invokeWithTarget:forwareder];
    } else {
        [super forwardInvocation:invocation];
    }
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    WCTValueForwarder *forwareder = [WCTValueForwarder forwarder];
    if ([forwareder respondsToForwardSelector:aSelector]) {
        return [forwareder methodSignatureForSelector:aSelector];
    } else {
        return [super methodSignatureForSelector:aSelector];
    }
}

@end
