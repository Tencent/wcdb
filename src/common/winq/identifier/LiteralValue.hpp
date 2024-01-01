//
// Created by sanhuazhang on 2019/05/02
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

#include "SQL.hpp"

namespace WCDB {

// BLOB is not supported since it may cause string truncation. Ones should use BindParameter instead.
class WCDB_API LiteralValue final : public SpecifiedSyntax<Syntax::LiteralValue, SQL> {
public:
    using SpecifiedSyntax<Syntax::LiteralValue, SQL>::SpecifiedSyntax;
    LiteralValue();
    ~LiteralValue() override;

    LiteralValue(int64_t value);
    LiteralValue(uint64_t value);
    LiteralValue(double value);
    LiteralValue(std::nullptr_t);
    LiteralValue(bool value);
    LiteralValue(const UnsafeStringView& value);

    template<typename T, typename Enable = typename std::enable_if<LiteralValueConvertible<T>::value>::type>
    LiteralValue(const T& t)
    {
        *this = LiteralValueConvertible<T>::asLiteralValue(t);
    }

    static LiteralValue currentTime();
    static LiteralValue currentDate();
    static LiteralValue currentTimestamp();
};

} // namespace WCDB
