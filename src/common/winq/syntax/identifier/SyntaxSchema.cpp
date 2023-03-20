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

namespace WCDB {

namespace Syntax {

bool Schema::isValid() const
{
    return !name.empty();
}

Schema::Schema() : name()
{
}

Schema::~Schema() = default;

#pragma mark - Identifier
Identifier::Type Schema::getType() const
{
    return type;
}

bool Schema::describle(std::ostream& stream) const
{
    stream << name;
    return true;
}

bool Schema::isMain() const
{
    return name.empty() || name == mainSchema;
}

bool Schema::isTemp() const
{
    return name == tempSchema;
}

bool Schema::empty() const
{
    return name.empty();
}

bool Schema::isTargetingSameSchema(const Schema& other) const
{
    return (isMain() && other.isMain()) || (getDescription() == other.getDescription());
}

} // namespace Syntax

} // namespace WCDB
