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

@interface WCTValueForwarder : NSObject

+ (WCTValueForwarder *)forwarder;

- (BOOL)respondsToForwardSelector:(SEL)aSelector;

@end

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

- (NSData *)dataValue
{
    return [self.stringValue dataUsingEncoding:NSUTF8StringEncoding];
}

- (NSNumber *)numberValue
{
    return self;
}

- (void)forwardInvocation:(NSInvocation *)invocation
{
    SEL aSelector = invocation.selector;
    NSObject *responser = self.stringValue;
    if (![responser respondsToSelector:aSelector]) {
        responser = self.dataValue;
        if (![responser respondsToSelector:aSelector]) {
            responser = nil;
        }
    }
    if (responser != nil) {
        [invocation invokeWithTarget:responser];
    } else {
        [super forwardInvocation:invocation];
    }
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    NSObject *forwarder = self.stringValue;
    if (![forwarder respondsToSelector:aSelector]) {
        forwarder = self.dataValue;
        if (![forwarder respondsToSelector:aSelector]) {
            forwarder = nil;
        }
    }
    if (forwarder != nil) {
        return [forwarder methodSignatureForSelector:aSelector];
    } else {
        return [super methodSignatureForSelector:aSelector];
    }
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

- (void)forwardInvocation:(NSInvocation *)invocation
{
    SEL aSelector = invocation.selector;
    NSObject *responser = self.stringValue;
    if (![responser respondsToSelector:aSelector]) {
        responser = self.numberValue;
        if (![responser respondsToSelector:aSelector]) {
            responser = nil;
        }
    }
    if (responser != nil) {
        [invocation invokeWithTarget:responser];
    } else {
        [super forwardInvocation:invocation];
    }
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    NSObject *forwarder = self.stringValue;
    if (![forwarder respondsToSelector:aSelector]) {
        forwarder = self.numberValue;
        if (![forwarder respondsToSelector:aSelector]) {
            forwarder = nil;
        }
    }
    if (forwarder != nil) {
        return [forwarder methodSignatureForSelector:aSelector];
    } else {
        return [super methodSignatureForSelector:aSelector];
    }
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

- (void)forwardInvocation:(NSInvocation *)invocation
{
    SEL aSelector = invocation.selector;
    NSObject *responser = self.numberValue;
    if (![responser respondsToSelector:aSelector]) {
        responser = self.dataValue;
        if (![responser respondsToSelector:aSelector]) {
            responser = nil;
        }
    }
    if (responser != nil) {
        [invocation invokeWithTarget:responser];
    } else {
        [super forwardInvocation:invocation];
    }
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    NSObject *forwarder = self.numberValue;
    if (![forwarder respondsToSelector:aSelector]) {
        forwarder = self.dataValue;
        if (![forwarder respondsToSelector:aSelector]) {
            forwarder = nil;
        }
    }
    if (forwarder != nil) {
        return [forwarder methodSignatureForSelector:aSelector];
    } else {
        return [super methodSignatureForSelector:aSelector];
    }
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

- (void)forwardInvocation:(NSInvocation *)invocation
{
    SEL aSelector = invocation.selector;
    WCTValueForwarder *forwarder = [WCTValueForwarder forwarder];
    if ([forwarder respondsToForwardSelector:aSelector]) {
        [invocation invokeWithTarget:forwarder];
    } else {
        [super forwardInvocation:invocation];
    }
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    WCTValueForwarder *forwarder = [WCTValueForwarder forwarder];
    if ([forwarder respondsToForwardSelector:aSelector]) {
        return [forwarder methodSignatureForSelector:aSelector];
    } else {
        return [super methodSignatureForSelector:aSelector];
    }
}

@end
