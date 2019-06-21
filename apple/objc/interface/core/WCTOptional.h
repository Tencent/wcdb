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
#import <optional>
#import <type_traits>

template<typename T>
class WCTIncompleteOptional {
    using Super = std::optional<T>;
    static_assert(!std::is_same<std::nullptr_t, T>::value, "");

public:
    virtual ~WCTIncompleteOptional() {}

    operator const T&() const { return value(); }

    const T& value() const
    {
        if (m_real.has_value()) {
            return m_real.value();
        }
        return m_default;
    }

    BOOL succeed() const { return m_real.has_value(); }
    BOOL failed() const { return !succeed(); }

    WCTIncompleteOptional& operator=(const T& value)
    {
        m_real = value;
        return *this;
    }

protected:
    WCTIncompleteOptional(const T& defaultValue)
    : m_real(std::nullopt), m_default(defaultValue)
    {
    }

    WCTIncompleteOptional(const T& value, const T& defaultValue)
    : m_real(value), m_default(defaultValue)
    {
    }

private:
    std::optional<T> m_real;
    T m_default;
};

template<typename T, T defaultValue>
class WCTFundamentalOptional final : public WCTIncompleteOptional<T> {
    using Super = WCTIncompleteOptional<T>;
    static_assert(std::is_fundamental<T>::value || std::is_enum<T>::value, "");

public:
    WCTFundamentalOptional() : Super(defaultValue) {}

    WCTFundamentalOptional(const std::nullopt_t&) : Super(defaultValue) {}

    WCTFundamentalOptional(const T& value) : Super(value, defaultValue) {}
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
