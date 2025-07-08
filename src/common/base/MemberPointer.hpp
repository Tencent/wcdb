//
// Created by qiuwenchen on 2022/8/26.
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
#include <type_traits>

namespace WCDB {

template<class T, typename Enable = void>
struct IsFieldMemberPointer : std::false_type {};

template<class T>
struct IsFieldMemberPointer<T, typename std::enable_if<std::is_member_pointer<T>::value && !std::is_member_function_pointer<T>::value>::type>
: std::true_type {};

template<typename T, typename O>
void* castMemberPointer(T O::* memberPointer)
{
    union {
        T O::* memberp;
        void* voidp;
    };
    voidp = nullptr;
    memberp = memberPointer;
    return voidp;
}

template<typename P>
struct getMemberType;

template<typename O, typename T>
struct getMemberType<T O::*> {
    typedef T type;
};

template<typename P>
struct getClassType;

template<typename O, typename T>
struct getClassType<T O::*> {
    typedef O type;
    typedef O* pointerType;
};

} //namespace WCDB
