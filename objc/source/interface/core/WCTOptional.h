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
#import <type_traits>

template<typename Value, Value defaultValue>
class WCTOptional final {
public:
    static_assert(std::is_default_constructible<Value>::value, "");
    static_assert(std::is_convertible<BOOL, Value>::value, "");

    WCTOptional(const Value& value) : m_value(value), m_failed(NO) {}

    WCTOptional(const std::nullptr_t& = nullptr) : m_failed(YES) {}

    operator BOOL() const
    {
        if (m_failed) {
            return NO;
        }
        return m_value;
    }

    const Value& value() const
    {
        if (m_failed) {
            return defaultValue;
        }
        return m_value;
    }

    BOOL failed() const { return m_failed; }

private:
    Value m_value;
    BOOL m_failed;
};
