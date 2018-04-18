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

#import <WCDB/Interface.h>
#import <WCDB/NSData+NoCopyData.h>

@implementation WCTValue {
    NSObject *_value;
}

- (instancetype)init
{
    if (self = [super init]) {
        _type = WCTColumnTypeNull;
        _value = nil;
    }
    return self;
}

- (instancetype)initWithNull
{
    return [self init];
}

- (instancetype)initWithInteger32:(int32_t)value
{
    if (self = [super init]) {
        _type = WCTColumnTypeInteger32;
        _value = [NSNumber numberWithInt:value];
    }
    return self;
}

- (instancetype)initWithInteger64:(int64_t)value
{
    if (self = [super init]) {
        _type = WCTColumnTypeInteger64;
        _value = [NSNumber numberWithLongLong:value];
    }
    return self;
}

- (instancetype)initWithCString:(const char *)value
{
    if (self = [super init]) {
        _type = WCTColumnTypeString;
        _value = [NSString stringWithUTF8String:value];
    }
    return self;
}

- (instancetype)initWithNoCopyData:(const WCDB::NoCopyData &)value
{
    if (self = [super init]) {
        _type = WCTColumnTypeBinary;
        _value = [NSData dataWithNoCopyData:value];
    }
    return self;
}

- (instancetype)initWithDouble:(double)value
{
    if (self = [super init]) {
        _type = WCTColumnTypeFloat;
        _value = [NSNumber numberWithDouble:value];
    }
    return self;
}

- (instancetype)initWithNumber:(NSNumber *)value
{
    if (self = [super init]) {
        _value = value;
        if (!value) {
            _type = WCTColumnTypeNull;
        } else if (CFNumberIsFloatType((CFNumberRef) value)) {
            _type = WCTColumnTypeFloat;
        } else {
            if (CFNumberGetByteSize((CFNumberRef) value) <= 4) {
                _type = WCTColumnTypeInteger32;
            } else {
                _type = WCTColumnTypeInteger64;
            }
        }
    }
    return self;
}

- (instancetype)initWithString:(NSString *)value
{
    if (self = [super init]) {
        _value = value;
        _type = value ? WCTColumnTypeString : WCTColumnTypeNull;
    }
    return self;
}

- (instancetype)initWithData:(NSData *)value
{
    if (self = [super init]) {
        _value = value;
        _type = value ? WCTColumnTypeBinary : WCTColumnTypeNull;
    }
    return self;
}

- (BOOL)boolValue
{
    switch (_type) {
        case WCTColumnTypeInteger32:
        case WCTColumnTypeInteger64:
        case WCTColumnTypeFloat:
            return ((NSNumber *) _value).boolValue;
        default:
            return NO;
    }
}

- (int32_t)integer32Value
{
    switch (_type) {
        case WCTColumnTypeInteger32:
        case WCTColumnTypeInteger64:
        case WCTColumnTypeFloat:
            return ((NSNumber *) _value).intValue;
        default:
            return 0;
    }
}

- (int64_t)integer64Value
{
    switch (_type) {
        case WCTColumnTypeInteger32:
        case WCTColumnTypeInteger64:
        case WCTColumnTypeFloat:
            return ((NSNumber *) _value).longLongValue;
        default:
            return 0;
    }
}

- (double)doubleValue
{
    switch (_type) {
        case WCTColumnTypeInteger32:
        case WCTColumnTypeInteger64:
        case WCTColumnTypeFloat:
            return ((NSNumber *) _value).doubleValue;
        default:
            return 0;
    }
}

- (NSString *)stringValue
{
    if (_type == WCTColumnTypeString) {
        return (NSString *) _value;
    }
    return nil;
}

- (NSNumber *)numberValue
{
    switch (_type) {
        case WCTColumnTypeInteger32:
        case WCTColumnTypeInteger64:
        case WCTColumnTypeFloat:
            return (NSNumber *) _value;
        default:
            return nil;
    }
}

- (NSData *)dataValue
{
    switch (_type) {
        case WCTColumnTypeBinary:
            return (NSData *) _value;
        default:
            return 0;
    }
}

- (id)forwardingTargetForSelector:(SEL)aSelector
{
    return _value;
}

+ (NSArray *)fundanmentalArrayFromValues:(NSArray<WCTValue *> *)values
{
    if (!values) {
        return nil;
    }
    NSMutableArray *fundamentalValues = [NSMutableArray array];
    for (WCTValue *value in values) {
        NSObject *fundamentalValue = nil;
        switch (value.type) {
            case WCTColumnTypeInteger32:
            case WCTColumnTypeInteger64:
            case WCTColumnTypeFloat:
                fundamentalValue = value.numberValue;
                break;
            case WCTColumnTypeString:
                fundamentalValue = value.stringValue;
                break;
            case WCTColumnTypeBinary:
                fundamentalValue = value.dataValue;
                break;
            default:
                break;
        }
        if (fundamentalValue) {
            [fundamentalValues addObject:fundamentalValue];
        }
    }
    return fundamentalValues;
}

- (NSString *)description
{
    return [_value description];
}

@end
