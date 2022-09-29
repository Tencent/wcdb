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

#include <WCDB/Assertion.hpp>
#include <WCDB/ObjectBridge.h>
#include <WCDB/Recyclable.hpp>

#define __WCDBGetObjectOrReturn(rawObj, objType, typedObjName, action)            \
    objType* typedObjName                                                         \
    = WCDB::ObjectBridge::extractOriginalCPPObject<objType>((rawObj).innerValue); \
    if (typedObjName == nullptr) {                                                \
        action;                                                                   \
    }

#define WCDBGetObjectOrReturn(rawObj, objType, typedObjName)                   \
    __WCDBGetObjectOrReturn(rawObj, objType, typedObjName, return )

#define WCDBGetObjectOrReturnValue(rawObj, objType, typedObjName, value)       \
    __WCDBGetObjectOrReturn(rawObj, objType, typedObjName, return value)

#define WCDBCreateCPPBridgedObject(objType, originObj)                         \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::createCPPObject(originObj))

#define WCDBCreateRecylableCPPObject(objType, originObj)                       \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::createCPPObject(originObj, true, true))

#define WCDBCreateUnmanageCPPObject(objType, originObj)                        \
    WCDB::ObjectBridge::createCPPBridgedObject<objType>(                       \
    WCDB::ObjectBridge::createCPPObject(originObj, false))

#define WCDBCreateSwiftBridgedObject(objType, originObj)                       \
    WCDB::ObjectBridge::createRecyclableSwiftObject<objType>(originObj)

#define WCDBGetSwiftObject(typedObj) typedObj.innerValue.get()

#define WCDBCreateSwiftBridgedClosure(objType, originObj)                      \
    WCDB::ObjectBridge::createRecyclableSwiftClosure<objType##ClosureType>(originObj)

#define WCDBGetSwiftClosure(typedObj) typedObj.get()
#define WCDBSwiftClosureCall(typedObj) WCDBGetSwiftClosure(typedObj)()
#define WCDBSwiftClosureCallWithOneArgument(typedObj, argument)                \
    WCDBGetSwiftClosure(typedObj)(argument)
#define WCDBSwiftClosureCallWithMultiArgument(typedObj, firstArgument, ...)    \
    WCDBGetSwiftClosure(typedObj)(firstArgument, nullptr, __VA_ARGS__)

namespace WCDB {

class ObjectBridge {
public:
    template<typename T>
    static T createRecyclableSwiftObject(SwiftObject* _Nonnull obj)
    {
        static_assert(offsetof(T, innerValue) == 0, "");

        auto recylableObj = Recyclable<SwiftObject*>(obj, [](SwiftObject* obj) {
            WCTAssert(WCDBReleaseSwiftObject != nullptr);
            WCDBReleaseSwiftObject(obj);
        });
        T ret;
        ret.innerValue = recylableObj;
        return ret;
    }

    template<typename T>
    static Recyclable<T> createRecyclableSwiftClosure(SwiftClosure* _Nullable obj)
    {
        return Recyclable<T>((T) obj, [](T obj) {
            if (obj != nullptr) {
                WCDBReleaseSwiftClosure((SwiftClosure*) obj);
            }
        });
    }

    template<typename T>
    static CPPObject* _Nonnull createCPPObject(T* _Nonnull obj,
                                               bool autoRelease = true,
                                               bool isRecyclable = false)
    {
        static_assert(!std::is_same<T, CPPObject>::value, "");
        CPPObject* cppObj = new CPPObject();
        cppObj->realValue = (void*) obj;
        cppObj->isRecyclableObj = isRecyclable;
        if (autoRelease) {
            cppObj->deleter = releaseCPPObject<T>;
        }
        return cppObj;
    }

    template<typename T>
    static CPPObject* _Nonnull createCPPObject(const T& obj,
                                               bool autoRelease = true,
                                               bool isRecyclable = false)
    {
        static_assert(!std::is_same<T, CPPObject>::value, "");
        static_assert(std::is_copy_constructible<T>::value, "");
        CPPObject* cppObj = new CPPObject();
        cppObj->realValue = new T(obj);
        cppObj->isRecyclableObj = isRecyclable;
        if (autoRelease) {
            cppObj->deleter = releaseCPPObject<T>;
        }
        return cppObj;
    }

    template<typename T>
    static CPPObject* _Nonnull createCPPObject(const T&& obj,
                                               bool autoRelease = true,
                                               bool isRecyclable = false)
    {
        static_assert(!std::is_same<T, CPPObject>::value, "");
        static_assert(std::is_copy_constructible<T>::value, "");
        CPPObject* cppObj = new CPPObject();
        cppObj->realValue = new T(std::move(obj));
        cppObj->isRecyclableObj = isRecyclable;
        if (autoRelease) {
            cppObj->deleter = releaseCPPObject<T>;
        }
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
            delete typedObj;
        }
    }

    template<typename T>
    static T* _Nullable extractOriginalCPPObject(const CPPObject* _Nonnull obj)
    {
        if (obj == nullptr) {
            return nullptr;
        }
        WCTAssert(obj->realValue != nullptr);
        T* typedObj = nullptr;
        if (!obj->isRecyclableObj) {
            typedObj = (T*) obj->realValue;
        } else {
            WCDB::Recyclable<T*>* recyclableObj = (WCDB::Recyclable<T*>*) obj->realValue;
            typedObj = recyclableObj->get();
        }
        return typedObj;
    }
};

} //namespace WCDB
