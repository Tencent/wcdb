//
// Created by qiuwenchen on 2022/8/27.
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

#include "ResultField.hpp"
#include "Accessor.hpp"
#include "Binding.hpp"
#include "Field.hpp"
#include "WINQ.h"

namespace WCDB {

ResultField::ResultField(const Field& field)
: ResultColumn(field), m_accessor(field.getAccessor())
{
}

ResultField::ResultField(const ResultColumn& resultColumn, const BaseAccessor* assessor)
: ResultColumn(resultColumn), m_accessor(assessor)
{
}

ResultField::~ResultField() = default;

void ResultField::configWithBinding(const Binding& binding, void* memberPointer)
{
    syntax().expression = Expression(binding.getColumnName(memberPointer));
    m_accessor = binding.getAccessor(memberPointer);
}

const BaseAccessor* ResultField::getAccessor() const
{
    return m_accessor;
}

ResultFields::~SyntaxList() = default;

ResultFields
ResultFields::resultColumnsByAddingNewResultColumns(const ResultFields& resultColumns) const
{
    ResultFields newResultColumns = *this;
    newResultColumns.insert(
    newResultColumns.end(), resultColumns.begin(), resultColumns.end());
    return newResultColumns;
}

ResultFields& ResultFields::addingNewResultColumns(const ResultFields& resultColumns)
{
    insert(end(), resultColumns.begin(), resultColumns.end());
    return *this;
}

bool ResultFields::isEqual(const ResultField& left, const ResultField& right)
{
    return left.getAccessor() == right.getAccessor()
           && left.getDescription().compare(right.getDescription()) == 0;
}

ResultFields
ResultFields::resultColumnsByRemovingResultColumns(const ResultFields& resultColumns) const
{
    ResultFields newResultColumns;
    for (const auto& resultColumn : *this) {
        if (std::find_if(resultColumns.begin(),
                         resultColumns.end(),
                         std::bind(&ResultFields::isEqual, resultColumn, std::placeholders::_1))
            == resultColumns.end()) {
            newResultColumns.push_back(resultColumn);
        }
    }
    return newResultColumns;
}

ResultFields& ResultFields::removingResultColumns(const ResultFields& resultColumns)
{
    for (const auto& resultColumn : resultColumns) {
        auto iter = std::find_if(
        begin(), end(), std::bind(&ResultFields::isEqual, resultColumn, std::placeholders::_1));
        if (iter != end()) {
            erase(iter);
        }
    }
    return *this;
}

} // namespace WCDB
