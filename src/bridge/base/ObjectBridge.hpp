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

#include "ObjectBridge.h"
#include "Recyclable.hpp"
#include <assert.h>
#include <stdlib.h>

#define __WCDBGetObjectOrReturn(rawObj, objType, typedObjName, action)              \
    objType* typedObjName                                                           \
    = (objType*) WCDB::ObjectBridge::extractOriginalCPPObject((rawObj).innerValue); \
    if (typedObjName == nullptr) {                                                  \
        action;                                                                     \
    }

#define WCDBGetObjectOrReturn(rawObj, objType, typedObjName)                   \
    __WCDBGetObjectOrReturn(rawObj, objType, typedObjName, return )

#define WCDBGetObjectOrReturnValue(rawObj, objType, typedObjName, value)       \
    __WCDBGetObjectOrReturn(rawObj, objType, typedObjName, return value)

#define WCDBCreateCPPBridgedObjectByCopy(objType, originObj)                   \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::copyCPPObject(originObj))

#define WCDBCreateRecylableCPPObject(objType, originObj)                       \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::copyCPPObject(originObj, true))

#define WCDBCreateCPPBridgedObject(objType, cppObjType)                        \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::createCPPObject<cppObjType>())

#define WCDBCreateCPPBridgedObjectWithParameters(objType, cppObjType, ...)     \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::createCPPObject<cppObjType>(__VA_ARGS__))

#define WCDBCreateUnmanagedCPPObject(objType, originObj)                       \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::createUnmanagedCPPObject((void*) originObj))

#define WCDBGetSwiftObject(typedObj) typedObj.innerValue.get()

#define WCDBGetBridgedData(type, data)                                         \
    (*((type*) WCDB::ObjectBridge::extractOriginalCPPObject((CPPObject*) data.intValue)))

#define WCDBGetCommonArrayLiteralValue(type, array, i) ((type*) array.buffer)[i]

#define WCDBGetCommonArrayObject(type, array, i)                               \
    (*((type*) WCDB::ObjectBridge::extractOriginalCPPObject(                   \
    (CPPObject*) ((long long*) array.buffer)[i])))

#define WCDBGetMultiTypeArrayObject(type, array, i)                            \
    (*((type*) WCDB::ObjectBridge::extractOriginalCPPObject(                   \
    (CPPObject*) (array.intValues[i]))))

namespace WCDB {

typedef Recyclable<void*> RecyclableContext;

class ObjectBridge {
public:
    static CPPObject* _Nullable createUnmanagedCPPObject(void* _Nullable obj);

    template<typename T>
    static CPPObject* _Nonnull copyCPPObject(T&& obj, bool isRecyclable = false)
    {
        static_assert(!std::is_same<T, CPPObject>::value, "");
        static_assert(std::is_copy_constructible<T>::value, "");
        static_assert(sizeof(CPPObject) <= 32, "");
        CPPObject* cppObj
        = (CPPObject*) malloc(32 + sizeof(std::remove_reference_t<T>));
        cppObj->realValue = (void*) ((unsigned long long) cppObj + 32);
        // Creating an object at an address that is not 16-byte aligned may crash on x86 systems.
        new (cppObj->realValue) std::remove_reference_t<T>(std::forward<T>(obj));
        cppObj->isRecyclableObj = isRecyclable;
        cppObj->deleter = releaseCPPObject<std::remove_reference_t<T>>;
        return cppObj;
    }

    template<typename T, typename... Args>
    static CPPObject* _Nonnull createCPPObject(const Args&... args)
    {
        static_assert(!std::is_same<T, CPPObject>::value, "");
        static_assert(sizeof(CPPObject) <= 32, "");
        CPPObject* cppObj = (CPPObject*) malloc(32 + sizeof(T));
        cppObj->realValue = (void*) ((unsigned long long) cppObj + 32);
        // Creating an object at an address that is not 16-byte aligned may crash on x86 systems.
        new (cppObj->realValue) T(args...);
        cppObj->isRecyclableObj = false;
        cppObj->deleter = releaseCPPObject<T>;
        return cppObj;
    }

    template<typename T>
    static T createCPPBridgedObject(CPPObject* _Nonnull obj)
    {
        static_assert(offsetof(T, innerValue) == 0, "");
        static_assert(sizeof(T) == sizeof(CPPObject*), "");

        T bridgeObj;
        bridgeObj.innerValue = obj;
        return bridgeObj;
    }

    template<typename T>
    static void releaseCPPObject(void* _Nonnull obj)
    {
        T* typedObj = (T*) obj;
        if (typedObj != nullptr) {
            typedObj->~T();
        }
    }

    static void* _Nullable extractOriginalCPPObject(const CPPObject* _Nonnull obj);
};

} //namespace WCDB
