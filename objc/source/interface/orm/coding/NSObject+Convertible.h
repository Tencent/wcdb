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

#import <Foundation/Foundation.h>
#import <WCDB/abstract.h>

namespace WCDB {

//NSObject
template <>
struct LiteralValueConvertible<NSObject *> : public std::true_type {
public:
    static LiteralValue asLiteralValue(NSObject *const &t);
};

template <>
struct ExpressionConvertible<NSObject *> : public std::true_type {
public:
    static Expression asExpression(NSObject *const &t);
};

template <>
struct ColumnResultConvertible<NSObject *> : public std::true_type {
public:
    static ColumnResult asColumnResult(NSObject *const &t);
};

template <>
struct OrderConvertible<NSObject *> : public std::true_type {
public:
    static Order asOrder(NSObject *const &t);
};

template <>
struct SpecificOrderConvertible<NSObject *> : public std::true_type {
public:
    static Order asOrder(NSObject *const &t, OrderTerm term);
};

//NSObject Subclass
template <typename T, typename Enable = void>
struct IsObjCType : std::false_type {
};

template <typename T>
struct IsObjCType<
    T,
    typename std::enable_if<std::is_convertible<NSObject *, T>::value &&
                            std::is_pointer<T>::value>::type> : std::true_type {
};

template <typename T>
struct LiteralValueConvertible<
    T,
    typename std::enable_if<IsObjCType<T>::value>::type>
    : public std::true_type {
public:
    static LiteralValue asLiteralValue(const T &t);
};

template <typename T>
struct ColumnResultConvertible<
    T,
    typename std::enable_if<IsObjCType<T>::value>::type>
    : public std::true_type {
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T>
struct OrderConvertible<T, typename std::enable_if<IsObjCType<T>::value>::type>
    : public std::true_type {
public:
    static Order asOrder(const T &t);
};

template <typename T>
struct SpecificOrderConvertible<
    T,
    typename std::enable_if<IsObjCType<T>::value>::type>
    : public std::true_type {
public:
    static Order asOrder(const T &t, OrderTerm term);
};

//NSObject Subclass Implementation
template <typename T>
LiteralValue
LiteralValueConvertible<T,
                        typename std::enable_if<IsObjCType<T>::value>::type>::
    asLiteralValue(const T &t)
{
    return LiteralValueConvertible<NSObject *>::asLiteralValue(t);
}

template <typename T>
ColumnResult
ColumnResultConvertible<T,
                        typename std::enable_if<IsObjCType<T>::value>::type>::
    asColumnResult(const T &t)
{
    return ColumnResultConvertible<NSObject *>::asColumnResult(t);
}

template <typename T>
Order OrderConvertible<T, typename std::enable_if<IsObjCType<T>::value>::type>::
    asOrder(const T &t)
{
    return OrderConvertible<NSObject *>::asOrder(t);
}

template <typename T>
Order SpecificOrderConvertible<
    T,
    typename std::enable_if<IsObjCType<T>::value>::type>::asOrder(const T &t,
                                                                  OrderTerm
                                                                      term)
{
    return OrderConvertible<NSObject *>::asOrder(t);
}

} //namespace WCDB
