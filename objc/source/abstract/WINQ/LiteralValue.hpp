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

#include <WCDB/Describable.hpp>
#include <sstream>

namespace WCDB {

class LiteralValue : public DescribableWithLang<Lang::LiteralValue> {
public:
    template <typename T>
    LiteralValue(
        const T &t,
        typename std::enable_if<ColumnIsNullType<T>::value>::type * = nullptr)
    {
        Lang::LiteralValue &lang = getMutableLang();
        lang.type = Lang::LiteralValue::Type::Null;
    }

    template <typename T>
    LiteralValue(
        const T &t,
        typename std::enable_if<ColumnIsFloatType<T>::value>::type * = nullptr)
    {
        Lang::LiteralValue &lang = getMutableLang();
        lang.type = Lang::LiteralValue::Type::NumbericFloat;
        std::ostringstream os;
        os << t;
        lang.stringValue.assign(os.str());
    }

    template <typename T>
    LiteralValue(const T &t,
                 typename std::enable_if<ColumnIsInteger32Type<T>::value>::type
                     * = nullptr)
    {
        Lang::LiteralValue &lang = getMutableLang();
        lang.type = Lang::LiteralValue::Type::NumbericInteger;
        lang.stringValue.assign(std::to_string(t));
    }

    template <typename T>
    LiteralValue(const T &t,
                 typename std::enable_if<ColumnIsInteger64Type<T>::value>::type
                     * = nullptr)
    {
        Lang::LiteralValue &lang = getMutableLang();
        lang.type = Lang::LiteralValue::Type::NumbericInteger;
        lang.stringValue.assign(std::to_string(t));
    }

    template <typename T>
    LiteralValue(
        const T &t,
        typename std::enable_if<ColumnIsTextType<T>::value>::type * = nullptr)
    {
        Lang::LiteralValue &lang = getMutableLang();
        lang.type = Lang::LiteralValue::Type::String;
        lang.stringValue.assign(ColumnIsTextType<T>::asUnderlyingType(t));
    }

    template <typename T>
    LiteralValue(
        const T &t,
        typename std::enable_if<ColumnIsBLOBType<T>::value>::type * = nullptr)
    {
        Lang::LiteralValue &lang = getMutableLang();
        lang.type = Lang::LiteralValue::Type::BLOB;
        Data data = ColumnIsBLOBType<T>::asUnderlyingType(t);
        data.makeShared();
        lang.dataValue = data;
    }

    static const LiteralValue currentTime();
    static const LiteralValue currentDate();
    static const LiteralValue currentTimestamp();

protected:
    LiteralValue(const Lang::LiteralValue::Type &type);
};

} // namespace WCDB

#endif /* LiteralValue_hpp */
