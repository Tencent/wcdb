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

#include <WCDB/Syntax.h>
#include <WCDB/SyntaxAssertion.hpp>

namespace WCDB {

namespace Syntax {

#pragma mark - Identifier
Identifier::Type LiteralValue::getType() const
{
    return type;
}

String LiteralValue::getValidDescription() const
{
    switch (switcher) {
    case Switch::String:
        return "'" + String(stringValue).replacingOccurrencesOfString("'", "''") + "'";
    case Switch::Null:
        return "NULL";
    case Switch::Float: {
        std::ostringstream stream;
        stream << floatValue;
        return stream.str();
    }
    case Switch::Integer:
        return std::to_string(integerValue);
    case Switch::UnsignedInteger:
        return std::to_string(unsignedIntegerValue);
    case Switch::Bool:
        return boolValue ? "TRUE" : "FALSE";
    case Switch::CurrentTime:
        return "CURRENT_TIME";
    case Switch::CurrentDate:
        return "CURRENT_DATE";
    case Switch::CurrentTimestamp:
        return "CURRENT_TIMESTAMP";
    }
}

} // namespace Syntax

} // namespace WCDB
