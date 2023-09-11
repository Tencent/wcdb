//
// Created by 陈秋文 on 2023/8/6.
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

#include "ContainerBridge.h"
#include "StringView.hpp"
#include "Value.hpp"

CPPCommonValue WCDBCreateCommonValue(const WCDB::Value& value)
{
    CPPCommonValue commonValue;
    switch (value.getType()) {
    case WCDB::Value::Type::Integer: {
        commonValue.type = WCDBBridgedType_Int;
        commonValue.intValue = value.intValue();
    } break;
    case WCDB::Value::Type::Float: {
        commonValue.type = WCDBBridgedType_Double;
        commonValue.doubleValue = value.floatValue();
    } break;
    case WCDB::Value::Type::Text: {
        commonValue.type = WCDBBridgedType_String;
        commonValue.intValue = reinterpret_cast<long long>(value.textValue().data());
    } break;
    default:
        commonValue.type = WCDBBridgedType_Invalid;
        break;
    }
    return commonValue;
}

void WCDBEnumerateStringViewMap(const void* _Nonnull map,
                                void* _Nonnull context,
                                StringViewMapEnumerator _Nonnull enumerator)
{
    const WCDB::StringViewMap<WCDB::Value>* cppMap
    = static_cast<const WCDB::StringViewMap<WCDB::Value>*>(map);
    for (auto iter = cppMap->begin(); iter != cppMap->end(); iter++) {
        enumerator(context, iter->first.data(), WCDBCreateCommonValue(iter->second));
    }
}
