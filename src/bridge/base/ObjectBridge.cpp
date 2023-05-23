//
// Created by qiuwenchen on 2022/3/31.
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

#include "ObjectBridge.hpp"

void (*_Nullable WCDBReleaseSwiftObject)(SwiftObject* _Nonnull obj) = nullptr;

void (*_Nullable WCDBReleaseSwiftClosure)(SwiftClosure* _Nonnull obj) = nullptr;

void WCDBReleaseCPPObject(CPPObject* _Nonnull obj)
{
    if (obj == nullptr) {
        return;
    }
    if (obj->deleter != nullptr) {
        obj->deleter(obj->realValue);
    }
    free(obj);
}

namespace WCDB {

CPPObject* _Nullable ObjectBridge::createUnmanagedCPPObject(void* _Nullable obj)
{
    if (obj == nullptr) {
        return nullptr;
    }
    CPPObject* cppObj = (CPPObject*) malloc(sizeof(CPPObject));
    cppObj->realValue = obj;
    cppObj->isRecyclableObj = false;
    cppObj->deleter = nullptr;
    return cppObj;
}

void* _Nullable ObjectBridge::extractOriginalCPPObject(const CPPObject* _Nonnull obj)
{
    if (obj == nullptr) {
        return nullptr;
    }
    WCTAssert(obj->realValue != nullptr);
    void* typedObj = nullptr;
    if (!obj->isRecyclableObj) {
        typedObj = (void*) obj->realValue;
    } else {
        WCDB::Recyclable<void*>* recyclableObj = (WCDB::Recyclable<void*>*) obj->realValue;
        typedObj = recyclableObj->get();
    }
    return typedObj;
}

} // namespace WCDB
