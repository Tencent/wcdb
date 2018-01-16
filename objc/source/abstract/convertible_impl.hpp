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

#ifndef convertible_impl_hpp
#define convertible_impl_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/literal_value.hpp>
#include <WCDB/subquery.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/column_result.hpp>
#include <WCDB/order.hpp>

namespace WCDB {
    
#pragma mark - Null Type
template <typename T>
LiteralValue LiteralValueConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type>::asLiteralValue(const T &t)
{
    return LiteralValue(t);
}
    
template <typename T>
Expression ExpressionConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type>::asExpression(const T &t) {
    return Expression(t);
}

template <typename T>
    ColumnResult ColumnResultConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type>::asColumnResult(const T &t)
{
    return ColumnResult(t);
}

template <typename T>
Order OrderConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type>::asOrder(const T& t)
{
    return Order(t);
}

template <typename T>
Order SpecificOrderConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type>::asOrder(const T& t, OrderTerm term)
{
    return Order(t, term);
}
    
#pragma mark - Float Type
template <typename T>
LiteralValue LiteralValueConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>::asLiteralValue(const T &t)
{
    return LiteralValue(t);
}
    
template <typename T>
Expression ExpressionConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>::asExpression(const T &t) {
    return Expression(t);
}

template <typename T>
ColumnResult ColumnResultConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>::asColumnResult(const T &t)
{
    return ColumnResult(t);
}

template <typename T>
Order OrderConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>::asOrder(const T& t)
{
    return Order(t);
}

template <typename T>
Order SpecificOrderConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>::asOrder(const T& t, OrderTerm term)
{
    return Order(t, term);
}

#pragma mark - Integer32 Type
template <typename T>
LiteralValue LiteralValueConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>::asLiteralValue(const T &t)
{
    return LiteralValue(t);
}
    
template <typename T>
Expression ExpressionConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>::asExpression(const T &t) {
    return Expression(t);
}

template <typename T>
ColumnResult ColumnResultConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>::asColumnResult(const T &t)
{
    return ColumnResult(t);
}

template <typename T>
Order OrderConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>::asOrder(const T& t)
{
    return Order(t);
}

template <typename T>
Order SpecificOrderConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>::asOrder(const T& t, OrderTerm term)
{
    return Order(t, term);
}

#pragma mark - Integer64 Type
template <typename T>
LiteralValue LiteralValueConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>::asLiteralValue(const T &t)
{
    return LiteralValue(t);
}
    
template <typename T>
Expression ExpressionConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>::asExpression(const T &t) {
    return Expression(t);
}

template <typename T>
ColumnResult ColumnResultConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>::asColumnResult(const T &t)
{
    return ColumnResult(t);
}

template <typename T>
Order OrderConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>::asOrder(const T& t)
{
    return Order(t);
}

template <typename T>
Order SpecificOrderConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>::asOrder(const T& t, OrderTerm term)
{
    return Order(t, term);
}

#pragma mark - Text Type
template <typename T>
LiteralValue LiteralValueConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>::asLiteralValue(const T &t)
{
    return LiteralValue(t);
}
    
template <typename T>
Expression ExpressionConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>::asExpression(const T &t) {
    return Expression(t);
}

template <typename T>
ColumnResult ColumnResultConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>::asColumnResult(const T &t)
{
    return ColumnResult(t);
}

template <typename T>
Order OrderConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>::asOrder(const T& t)
{
    return Order(t);
}

template <typename T>
Order SpecificOrderConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>::asOrder(const T& t, OrderTerm term)
{
    return Order(t, term);
}
    
template <typename T>
Subquery TableOrSubqueryConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>::asTableOrSubquery(const T &t)
{
    return Subquery(ColumnIsTextType<T>::asUnderlyingType(t));
}

#pragma mark - BLOB Type
template <typename T>
LiteralValue LiteralValueConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>::asLiteralValue(const T &t)
{
    return LiteralValue(t);
}
    
template <typename T>
Expression ExpressionConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>::asExpression(const T &t) {
    return Expression(t);
}

template <typename T>
ColumnResult ColumnResultConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>::asColumnResult(const T &t)
{
    return ColumnResult(t);
}

template <typename T>
Order OrderConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>::asOrder(const T& t)
{
    return Order(t);
}

template <typename T>
Order SpecificOrderConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>::asOrder(const T& t, OrderTerm term)
{
    return Order(t, term);
}

} //namespace WCDB

#endif /* convertible_impl_hpp */
