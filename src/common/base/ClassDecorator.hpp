//
// Created by 陈秋文 on 2023/11/15.
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

#include "FunctionContainer.hpp"
#include "StringView.hpp"
#include <list>
#include <memory>

#define WCDBRegisterDecorativeFunction(baseClass, func)                        \
    auto lambda_##func = [this](auto&&... args) -> decltype(auto) {            \
        return this->baseClass::func(decltype(args)(args)...);                 \
    };                                                                         \
    registerFunction<getMemberType<decltype(&baseClass::func)>::type>(         \
    SlotType::func, lambda_##func);

#define WCDBCallDecoratedFunction(type, func, ...)                             \
    getFunction(SlotType::func, &type::func)(__VA_ARGS__)

#define WCDBCallDecorativeFunction(baseClass, func, ...)                       \
    (!m_hasDecorators ? baseClass::func(__VA_ARGS__) :                         \
                        getFunction(SlotType::func, &baseClass::func)(__VA_ARGS__))

#define WCDBAssertFunctionTypeConsistent(classA, classB, memberFunction)         \
    static_assert(                                                               \
    std::is_same<getMemberType<decltype(&classA::memberFunction)>::type,         \
                 getMemberType<decltype(&classB::memberFunction)>::type>::value, \
    "Function types must be consistent.");

#define WCDBSwizzleDecorativeFunction(object, curClass, func)                               \
    registerFunction(SlotType::func, object->getFunction(SlotType::func, &curClass::func)); \
    auto lambda_##func = [this](auto&&... args) -> decltype(auto) {                         \
        return this->curClass::func(decltype(args)(args)...);                               \
    };                                                                                      \
    object->registerFunction<getMemberType<decltype(&curClass::func)>::type>(               \
    SlotType::func, lambda_##func);

namespace WCDB {

template<typename Type, unsigned char Count>
class DecorativeClass;

template<typename Type, unsigned char Count>
class ClassDecorator : public FunctionContainer<Type> {
public:
    ClassDecorator() { FunctionContainerBase::initFunctionSlot(Count); }

    typedef DecorativeClass<Type, Count> Decorative;
    virtual void decorate(Decorative* decorativeClass)
    {
        m_decoratedObj = decorativeClass;
    }

protected:
    DecorativeClass<Type, Count>* m_decoratedObj = nullptr;
};

template<typename Type, unsigned char Count>
class DecorativeClass : public FunctionContainer<Type> {
public:
    typedef ClassDecorator<Type, Count> Decorator;

    virtual void initDecorativeFunctions() = 0;

    template<typename SpecifiedDecorator, typename... Args>
    void tryAddDecorator(const UnsafeStringView& name, Args&... args)
    {
        static_assert(std::is_base_of<Decorator, SpecifiedDecorator>::value, "");
        if (containDecorator(name)) {
            return;
        }
        std::shared_ptr<Decorator> decorator
        = std::make_shared<SpecifiedDecorator>(args...);
        addDecorator(name, decorator);
    }

    template<typename SpecifiedDecorator, typename... Args>
    void tryAddDecorator(const UnsafeStringView& name, const Args&... args)
    {
        static_assert(std::is_base_of<Decorator, SpecifiedDecorator>::value, "");
        if (containDecorator(name)) {
            return;
        }
        std::shared_ptr<Decorator> decorator
        = std::make_shared<SpecifiedDecorator>(args...);
        addDecorator(name, decorator);
    }

    virtual void onDecorationChange() {}

    bool containDecorator(const UnsafeStringView& name)
    {
        return m_decorators.find(name) != m_decorators.end();
    }

    template<typename SpecifiedDecorator>
    SpecifiedDecorator* getDecorator(const UnsafeStringView& name)
    {
        static_assert(std::is_base_of<Decorator, SpecifiedDecorator>::value, "");
        auto iter = m_decorators.find(name);
        if (iter == m_decorators.end()) {
            return nullptr;
        }
        WCTAssert(dynamic_cast<SpecifiedDecorator*>(iter->second.get()) != nullptr);
        return static_cast<SpecifiedDecorator*>(iter->second.get());
    }

    void clearDecorators()
    {
        m_hasDecorators = false;
        FunctionContainerBase::clearFunctionSlot();
        m_decorators.clear();
        onDecorationChange();
    }

protected:
    bool m_hasDecorators = false;

private:
    void addDecorator(const UnsafeStringView& name, std::shared_ptr<Decorator> decorator)
    {
        WCTAssert(!containDecorator(name));
        if (!m_hasDecorators) {
            FunctionContainerBase::initFunctionSlot(Count);
            initDecorativeFunctions();
        }
        m_hasDecorators = true;
        m_decorators.emplace(name, decorator);
        decorator->decorate(this);
        onDecorationChange();
    }
    StringViewMap<std::shared_ptr<Decorator>> m_decorators;
};

} //namespace WCDB
