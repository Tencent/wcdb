//
// Created by sanhuazhang on 2019/05/02
//

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
#import <WCDB/WCTConvertible.h>

namespace WCDB {

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<NSString*>::asUnderlyingType(NSString* text)
{
    return UnsafeStringView(text.UTF8String, [text lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<NSMutableString*>::asUnderlyingType(NSMutableString* text)
{
    return UnsafeStringView(text.UTF8String, [text lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<NSData*>::asUnderlyingType(NSData* data)
{
    return UnsafeData((unsigned char*) data.bytes, (size_t) data.length);
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<NSMutableData*>::asUnderlyingType(NSMutableData* data)
{
    return UnsafeData((unsigned char*) data.bytes, (size_t) data.length);
}

Expression ExpressionConvertible<WCTProperty>::asExpression(const WCTProperty& property)
{
    return Expression((const Column&) property);
}

IndexedColumn IndexedColumnConvertible<WCTProperty>::asIndexedColumn(const WCTProperty& property)
{
    return Expression((const Column&) property);
}

LiteralValue LiteralValueConvertible<NSString*>::asLiteralValue(NSString* string)
{
    if (string == nil) {
        return LiteralValue(nullptr);
    }
    return UnsafeStringView(string.UTF8String, [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
}

LiteralValue LiteralValueConvertible<NSMutableString*>::asLiteralValue(NSMutableString* string)
{
    if (string == nil) {
        return LiteralValue(nullptr);
    }
    return UnsafeStringView(string.UTF8String, [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
}

LiteralValue LiteralValueConvertible<NSNumber*>::asLiteralValue(NSNumber* number)
{
    if (number == nil) {
        return LiteralValue(nullptr);
    }
    if (number == nil || CFNumberIsFloatType((CFNumberRef) number)) {
        return (double) number.doubleValue;
    } else if (strcmp(number.objCType, "Q") == 0) {
        // unsigned long long
        return (uint64_t) number.unsignedLongLongValue;
    }
    return (int64_t) number.integerValue;
}

#if OBJC_BOOL_IS_CHAR
LiteralValue LiteralValueConvertible<BOOL>::asLiteralValue(BOOL value)
{
    return LiteralValue((bool) value);
}
#endif // OBJC_BOOL_IS_CHAR

Expressions _SyntaxList<Expression>::Convertible<NSArray*>::asSyntaxList(const NSArray* values)
{
    Expressions result;
    for (id value in values) {
        if ([value isKindOfClass:NSString.class]) {
            NSString* stringValue = (NSString*) value;
            result.push_back(stringValue);
        } else if ([value isKindOfClass:NSNumber.class]) {
            NSNumber* number = (NSNumber*) value;
            result.push_back(number);
        } else {
            NSCAssert(false, @"Only supports converting NSNumber and NSString in array into expressions");
        }
    }
    return result;
}

#define WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(Type)                               \
    Expressions _SyntaxList<Expression>::Convertible<Type>::asSyntaxList(const Type values) \
    {                                                                                       \
        return _SyntaxList<Expression>::Convertible<NSArray*>::asSyntaxList(values);        \
    }

WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(NSArray<NSString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(NSArray<NSMutableString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(NSArray<NSNumber*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(NSMutableArray*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(NSMutableArray<NSString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(NSMutableArray<NSMutableString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_ARRAY(NSMutableArray<NSNumber*>*)

Expressions _SyntaxList<Expression>::Convertible<NSSet*>::asSyntaxList(const NSSet* values)
{
    Expressions result;
    for (id value in values) {
        if ([value isKindOfClass:NSString.class]) {
            NSString* stringValue = (NSString*) value;
            result.push_back(stringValue);
        } else if ([value isKindOfClass:NSNumber.class]) {
            NSNumber* number = (NSNumber*) value;
            result.push_back(number);
        } else {
            NSCAssert(false, @"Only supports converting NSNumber and NSString in set into expressions");
        }
    }
    return result;
}

#define WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(Type)                                 \
    Expressions _SyntaxList<Expression>::Convertible<Type>::asSyntaxList(const Type values) \
    {                                                                                       \
        return _SyntaxList<Expression>::Convertible<NSSet*>::asSyntaxList(values);          \
    }

WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(NSSet<NSString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(NSSet<NSMutableString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(NSSet<NSNumber*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(NSMutableSet*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(NSMutableSet<NSString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(NSMutableSet<NSMutableString*>*)
WCDB_IMPLEMENT_EXPRESSIONS_CONVERTIBLE_OF_Set(NSMutableSet<NSNumber*>*)

}
