//
// Created by qiuwenchen on 2023/11/14.
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

#include "Assertion.hpp"
#include "Macro.h"
#include "MemberPointer.hpp"
#include <functional>
#include <string>

namespace WCDB {

class FunctionContainerBase {
public:
    typedef std::function<void()> Slot;
    typedef void (*Destructor)(Slot*);

    FunctionContainerBase();
    FunctionContainerBase(FunctionContainerBase&& other);
    FunctionContainerBase& operator=(FunctionContainerBase&&);
    FunctionContainerBase(const FunctionContainerBase& other) = delete;
    FunctionContainerBase& operator=(const FunctionContainerBase&) = delete;
    virtual ~FunctionContainerBase();

protected:
    template<typename FuncType>
    static void functionDestructor(Slot* func)
    {
        std::function<FuncType>* typedFunc = (std::function<FuncType>*) func;
        typedef std::function<FuncType> STDFunctionType;
        typedFunc->~STDFunctionType();
    }

    void initFunctionSlot(size_t slotCount);
    size_t getSlotCount() const;
    void clearFunctionSlot();

    Slot* m_functionSlots;
    Destructor* m_funcDeconstructors;
};

template<typename Type>
class FunctionContainer : public FunctionContainerBase {
public:
    typedef Type SlotType;

    template<typename FuncType>
    void registerFunction(SlotType slot, const std::function<FuncType>& func)
    {
        static_assert(sizeof(std::function<FuncType>) == sizeof(Slot),
                      "Unsupported function type!");
        int slotNum = (int) slot;
        if (m_funcDeconstructors[slotNum] != nullptr) {
            m_funcDeconstructors[slotNum](m_functionSlots + slotNum);
        }
        new ((void*) (m_functionSlots + slotNum)) std::function<FuncType>(func);
        m_funcDeconstructors[slotNum] = functionDestructor<FuncType>;
    }

    template<typename FuncType, typename ObjectType>
    const std::function<FuncType>& getFunction(SlotType slot, FuncType ObjectType::*)
    {
        static_assert(std::is_member_function_pointer<FuncType ObjectType::*>::value,
                      "It must be a member function!");
        WCTAssert((size_t) slot < getSlotCount()
                  && m_funcDeconstructors[(int) slot] != nullptr);
        return *(std::function<FuncType>*) (m_functionSlots + (int) slot);
    }
};

} // namespace WCDB
