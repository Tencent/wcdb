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

#include "Syntax.h"
#include "SyntaxAssertion.hpp"
#include <limits>

namespace WCDB {

namespace Syntax {

LiteralValue::~LiteralValue() = default;

#pragma mark - Identifier
Identifier::Type LiteralValue::getType() const
{
    return type;
}

bool LiteralValue::describle(std::ostream& stream) const
{
    switch (switcher) {
    case Switch::StringView: {
        stream << "'";
        for (int i = 0; i < stringValue.length(); i++) {
            const auto character = stringValue.at(i);
            if (character != '\'') {
                stream << character;
            } else {
                stream << "''";
            }
        }
        stream << "'";
    } break;
    case Switch::Null:
        stream << "NULL";
        break;
    case Switch::Float:
        stream.precision(std::numeric_limits<double>::max_digits10);
        stream << floatValue;
        break;
    case Switch::Integer:
        stream << integerValue;
        break;
    case Switch::UnsignedInteger:
        stream << unsignedIntegerValue;
        break;
    case Switch::Bool:
        stream << (boolValue ? "TRUE" : "FALSE");
        break;
    case Switch::CurrentTime:
        stream << "CURRENT_TIME";
        break;
    case Switch::CurrentDate:
        stream << "CURRENT_DATE";
        break;
    case Switch::CurrentTimestamp:
        stream << "CURRENT_TIMESTAMP";
        break;
    }
    return true;
}

} // namespace Syntax

} // namespace WCDB
