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

namespace WCDB {

LiteralValue LiteralValueConvertible<NSObject*>::asLiteralValue(NSObject* const &t)
{
    WCTValue* value = [(NSObject<WCTColumnCoding>*)t archivedWCTValue];
    WCTColumnType columnType = [((NSObject<WCTColumnCoding>*)t).class columnTypeForWCDB];
    switch (columnType) {
        case WCTColumnTypeNull:
            return LiteralValue(nullptr);
        case WCTColumnTypeInteger32:
            return LiteralValue(((NSNumber*)value).intValue);
        case WCTColumnTypeInteger64:
            return LiteralValue(((NSNumber*)value).longLongValue);
        case WCTColumnTypeDouble:
            return LiteralValue(((NSNumber*)value).doubleValue);
        case WCTColumnTypeString:
            return LiteralValue(((NSString*)value).UTF8String);
        case WCTColumnTypeBinary: {
            NSData *data = (NSData *) value;
            const unsigned char* raw = (const unsigned char*)data.bytes;
            std::vector<unsigned char> vector(raw, raw + data.length);
            return LiteralValue(vector);
        }
    }
}  

Expression ExpressionConvertible<NSObject*>::asExpression(NSObject* const &t)
{
    return Expression(LiteralValueConvertible<NSObject*>::asLiteralValue(t));
}

ColumnResult ColumnResultConvertible<NSObject*>::asColumnResult(NSObject* const &t)
{
    return ColumnResult(ExpressionConvertible<NSObject*>::asExpression(t));
}

Order OrderConvertible<NSObject*>::asOrder(NSObject* const & t)
{
    return Order(ExpressionConvertible<NSObject*>::asExpression(t));
}

Order SpecificOrderConvertible<NSObject*>::asOrder(NSObject* const & t, OrderTerm term) 
{
    return Order(ExpressionConvertible<NSObject*>::asExpression(t), term);
}

} //namespace WCDB
