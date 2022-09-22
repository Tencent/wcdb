//
// Created by qiuwenchen on 2022/3/30.
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

#pragma once

#include <WCDB/Macro.h>

WCDB_EXTERN_C_BEGIN

typedef struct SwiftObject SwiftObject;
extern void (*_Nullable WCDBReleaseSwiftObject)(SwiftObject* _Nonnull obj);

typedef struct SwiftClosure SwiftClosure;
extern void (*_Nullable WCDBReleaseSwiftClosure)(SwiftClosure* _Nonnull obj);

typedef struct CPPObject {
    void* _Nonnull realValue;
    void (*_Nullable deleter)(void* _Nonnull);
    bool isRecyclableObj;
} CPPObject;
void WCDBReleaseCPPObject(CPPObject* _Nonnull obj);

#define WCDBDefineSwiftObjectBridgedType(typename)                             \
    typedef struct typename                                                    \
    {                                                                          \
        WCDB::Recyclable<SwiftObject*> innerValue;                             \
    }                                                                          \
    typename;

#define WCDBDefineMultiArgumentSwiftClosureBridgedType(typename, returnType, firstArgType, ...) \
    static_assert(!std::is_floating_point<firstArgType>::value,                                 \
                  "First argument should not be float");                                        \
    static_assert(sizeof(firstArgType) <= 8, "Size of first argument is limited to 8 byte");    \
    typedef returnType (*typename##ClosureType)(firstArgType, void*, __VA_ARGS__);              \
    typedef WCDB::Recyclable<typename##ClosureType> typename;

#define WCDBDefineOneArgumentSwiftClosureBridgedType(typename, returnType, firstArgType)     \
    static_assert(!std::is_floating_point<firstArgType>::value,                              \
                  "First argument should no be float");                                      \
    static_assert(sizeof(firstArgType) <= 8, "Size of first argument is limited to 8 byte"); \
    typedef returnType (*typename##ClosureType)(firstArgType);                               \
    typedef WCDB::Recyclable<typename##ClosureType> typename;

#define WCDBDefineNoArgumentSwiftClosureBridgedType(typename, returnType)      \
    typedef returnType (*typename##ClosureType)();                             \
    typedef WCDB::Recyclable<typename##ClosureType> typename;

#define WCDBDefineCPPBridgedType(typename)                                     \
    typedef struct typename                                                    \
    {                                                                          \
        CPPObject* _Nullable innerValue;                                       \
    }                                                                          \
    typename;

#define WCDBDefineOptionalBridgedType(bridgedTypeName, originalTypeName)       \
    typedef struct bridgedTypeName {                                           \
        bool hasValue;                                                         \
        originalTypeName value;                                                \
    } bridgedTypeName;

WCDBDefineOptionalBridgedType(OptionalBool, bool)
WCDBDefineOptionalBridgedType(OptionalUInt64, unsigned long long)

WCDB_EXTERN_C_END
