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

#import <WCDB/NSObject+Convertible.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTValue.h>
#import <WCDB/error.hpp>

namespace WCDB {

LiteralValue LiteralValueConvertible<NSObject *>::asLiteralValue(NSObject *const &t)
{
    WCTValue *value = t;
    WCTValueType valueType = [value valueType];
    if (valueType == WCTValueTypeColumnCoding) {
        value = [(id<WCTColumnCoding>) value archivedWCTValue];
        valueType = [value valueType];
    }
    switch (valueType) {
        case WCTValueTypeString:
            return LiteralValue(((NSString *) value).UTF8String);
        case WCTValueTypeNumber: {
            NSNumber *number = (NSNumber *) value;
            if (CFNumberIsFloatType((CFNumberRef) number)) {
                return LiteralValue(number.doubleValue);
            } else {
                if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
                    return LiteralValue(number.intValue);
                } else {
                    return LiteralValue(number.longLongValue);
                }
            }
        } break;
        case WCTValueTypeData: {
            NSData *data = (NSData *) value;
            const unsigned char *raw = (const unsigned char *) data.bytes;
            std::vector<unsigned char> vector(raw, raw + data.length);
            return LiteralValue(vector);
        } break;
        case WCTValueTypeNil:
            return LiteralValue(nullptr);
            break;
        default:
            WCDB::Error::Abort(([NSString stringWithFormat:@"Converting LiteralValue with unknown type %@", NSStringFromClass(value.class)].UTF8String));
            break;
    }
    return LiteralValue("");
}

Expression ExpressionConvertible<NSObject *>::asExpression(NSObject *const &t)
{
    return Expression(LiteralValueConvertible<NSObject *>::asLiteralValue(t));
}

ColumnResult ColumnResultConvertible<NSObject *>::asColumnResult(NSObject *const &t)
{
    return ColumnResult(ExpressionConvertible<NSObject *>::asExpression(t));
}

Order OrderConvertible<NSObject *>::asOrder(NSObject *const &t)
{
    return Order(ExpressionConvertible<NSObject *>::asExpression(t));
}

Order SpecificOrderConvertible<NSObject *>::asOrder(NSObject *const &t, OrderTerm term)
{
    return Order(ExpressionConvertible<NSObject *>::asExpression(t), term);
}

} //namespace WCDB
