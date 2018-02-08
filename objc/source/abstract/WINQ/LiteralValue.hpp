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

#ifndef LiteralValue_hpp
#define LiteralValue_hpp

#include <WINQ/Describable.hpp>

namespace WCDB {

class LiteralValue : public DescribableWithLang<lang::LiteralValue> {
public:
    template <typename T>
    LiteralValue(const T &t,
                 typename std::enable_if<std::is_floating_point<T>::value>::type
                     * = nullptr)
    {
        lang::LiteralValue &lang = getMutableLang();
        lang.type = lang::LiteralValue::Type::NumbericFloat;
        lang.floatValue = t;
    }

    template <typename T>
    LiteralValue(const T &t,
                 typename std::enable_if<(std::is_integral<T>::value ||
                                          std::is_enum<T>::value)> * = nullptr)
    {
        lang::LiteralValue &lang = getMutableLang();
        lang.type = lang::LiteralValue::Type::NumbericInteger;
        lang.integerValue = (int64_t) t;
    }

    LiteralValue(const std::string &value);
    LiteralValue(const std::vector<unsigned char> &value);

    static const LiteralValue Null;
    static const LiteralValue CurrentTime;
    static const LiteralValue CurrentDate;
    static const LiteralValue CurrentTimestamp;

protected:
    LiteralValue(const lang::LiteralValue::Type &type);
};

} // namespace WCDB

#endif /* LiteralValue_hpp */
