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

#import <Foundation/Foundation.h>
#import <WCDB/WCTConvertible.h>
#import <type_traits>

template<typename T>
class WCTIncompleteOptional {
public:
    WCTIncompleteOptional(const T& value, BOOL isOK)
    : m_value(value), m_isOK(isOK)
    {
    }

    virtual ~WCTIncompleteOptional() {}

    operator const T&() const { return m_value; }

    const T& value() const { return m_value; }

    BOOL failed() const { return !m_isOK; }

    WCTIncompleteOptional& operator=(const T& value)
    {
        m_isOK = YES;
        m_value = value;
        return *this;
    }

private:
    T m_value;
    BOOL m_isOK;
};

template<typename T, T defaultValue>
class WCTFundamentalOptional final : public WCTIncompleteOptional<T> {
    static_assert(std::is_fundamental<T>::value || std::is_enum<T>::value, "");
    static_assert(!std::is_same<std::nullptr_t, T>::value, "");

public:
    WCTFundamentalOptional() : WCTIncompleteOptional<T>(defaultValue, NO) {}

    WCTFundamentalOptional(const T& value)
    : WCTIncompleteOptional<T>(value, YES)
    {
    }

    WCTFundamentalOptional(const std::nullptr_t&)
    : WCTIncompleteOptional<T>(defaultValue, NO)
    {
    }
};

//template<typename T>
//class WCTObjCOptional : public WCTIncompleteOptional<T> {
//    static_assert(WCDB::IsObjCType<T>::value, "");
//public:
//    WCTObjCOptional() : WCTIncompleteOptional<T>(nil, YES) {}
//
//    WCTObjCOptional(T value) : WCTIncompleteOptional<T>(value, YES) {}
//
//    WCTObjCOptional(const std::nullptr_t&) : WCTIncompleteOptional<T>(nil, NO)
//    {
//    }
//};
