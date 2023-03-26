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

#import "WINQ.h"
#import <Foundation/Foundation.h>

class WCTProperty;

namespace WCDB {

//NSObject Subclass
template<typename T, typename Enable = void>
struct IsObjCType : std::false_type {
};

template<typename T>
struct IsObjCType<
T,
typename std::enable_if<std::is_convertible<NSObject *, T>::value && std::is_pointer<T>::value>::type> : std::true_type {
};

template<>
struct ColumnIsTextType<NSString *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(NSString *text);
};

template<>
struct ColumnIsTextType<NSMutableString *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(NSMutableString *text);
};

template<>
class LiteralValueConvertible<NSString *> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(NSString *string);
};

template<>
class LiteralValueConvertible<NSMutableString *> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(NSMutableString *string);
};

template<>
class LiteralValueConvertible<NSNumber *> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(NSNumber *number);
};

#if OBJC_BOOL_IS_CHAR
template<>
class LiteralValueConvertible<BOOL> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(BOOL value);
};
#endif // OBJC_BOOL_IS_CHAR

template<>
struct ColumnIsBLOBType<NSData *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(NSData *data);
};

template<>
struct ColumnIsBLOBType<NSMutableData *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(NSMutableData *data);
};

template<>
class ExpressionConvertible<WCTProperty> final : public std::true_type {
public:
    static Expression asExpression(const WCTProperty &property);
};

template<>
class IndexedColumnConvertible<WCTProperty> final : public std::true_type {
public:
    static IndexedColumn asIndexedColumn(const WCTProperty &property);
};

#define WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(Type)                              \
    template<>                                                                 \
    template<>                                                                 \
    struct _SyntaxList<Expression>::Convertible<Type> final : std::true_type { \
        static Expressions asSyntaxList(const Type);                           \
    };

WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSArray *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSArray<NSString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSArray<NSMutableString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSArray<NSNumber *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableArray *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableArray<NSString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableArray<NSMutableString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableArray<NSNumber *> *)

WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSSet *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSSet<NSString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSSet<NSMutableString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSSet<NSNumber *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableSet *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableSet<NSString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableSet<NSMutableString *> *)
WCDB_DEFINE_EXPRESSIONS_CONVERTIBLE(NSMutableSet<NSNumber *> *)

template<typename T>
struct remove_ownership {
    typedef T type;
};
#if __has_feature(objc_arc)
template<typename T>
struct remove_ownership<__unsafe_unretained T> {
    typedef T type;
};
template<typename T>
struct remove_ownership<__strong T> {
    typedef T type;
};
template<typename T>
struct remove_ownership<__weak T> {
    typedef T type;
};
#endif
template<typename T>
using remove_ownership_t = typename remove_ownership<T>::type;

} //namespace WCDB
