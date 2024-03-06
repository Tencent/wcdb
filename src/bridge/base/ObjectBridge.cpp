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
#include "Assertion.hpp"

void (*_Nullable WCDBReleaseSwiftObject)(SwiftObject* _Nonnull obj) = nullptr;

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

#ifdef __ANDROID__

char* _Nullable* _Nullable WCDBPreAllocStringMemorySlot(int count)
{
    return WCDB::StringView::preAllocStringMemorySlot(count);
}

void WCDBAllocStringMemory(char* _Nullable* _Nullable slot, int size)
{
    WCDB::StringView::allocStringMemory(slot, size);
}

void WCDBClearAllocatedMemory(int count)
{
    WCDB::StringView::clearAllocatedMemory(count);
}

void WCDBClearAllPreAllocatedMemory()
{
    WCDB::StringView::clearAllPreAllocatedMemory();
}

#endif

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
