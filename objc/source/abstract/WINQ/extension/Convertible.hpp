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

#ifndef Convertible_hpp
#define Convertible_hpp

#include <WCDB/ColumnType.hpp>
#include <WCDB/SyntaxForwardDeclaration.h>

namespace WCDB {

template<typename T, typename Enable = void>
class LiteralValueConvertible : public std::false_type {
public:
    static LiteralValue asLiteralValue(const T&);
};

template<typename T, typename Enable = void>
class ExpressionConvertible : public std::false_type {
public:
    static Expression asExpression(const T&);
};

template<typename T, typename Enable = void>
class IndexedColumnConvertible : public std::false_type {
public:
    static IndexedColumn asIndexedColumn(const T&);
};

template<typename T, typename Enable = void>
class ResultColumnConvertible : public std::false_type {
public:
    static ResultColumn asResultColumn(const T&);
};

template<typename T, typename Enable = void>
class QualifiedTableConvertible : public std::false_type {
public:
    static QualifiedTable asQualifiedTable(const T&);
};

template<typename T, typename Enable = void>
class CTETableConvertible : public std::false_type {
public:
    static CTETable asCTETable(const T&);
};

template<typename T, typename Enable = void>
class OrderingTermConvertible : public std::false_type {
public:
    static OrderingTerm asOrderingTerm(const T&);
};

template<typename T, typename Enable = void>
class TableOrSubqueryConvertible : public std::false_type {
public:
    static TableOrSubquery asTableOrSubquery(const T&);
};

} // namespace WCDB

#endif /* Convertible_hpp */
