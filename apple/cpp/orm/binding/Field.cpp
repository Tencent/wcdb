//
// Created by qiuwenchen on 2022/8/26.
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

#include <WCDB/Accessor.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/Binding.hpp>
#include <WCDB/Field.hpp>
#include <WCDB/ResultField.hpp>

namespace WCDB {

#pragma mark - Field

Field::Field(const UnsafeStringView& name, const std::shared_ptr<BaseAccessor> accessor)
: Column(name), m_accessor(accessor)
{
}

Field::~Field() = default;

std::shared_ptr<BaseAccessor> Field::getAccessor() const
{
    return m_accessor;
}

void Field::configWithBinding(const Binding& binding, void* memberPointer)
{
    syntax().name = binding.getColumnName(memberPointer);
    m_accessor = binding.getAccessor(memberPointer);
}

Column Field::table(const UnsafeStringView& table) const
{
    return Column(*this).table(table);
}

#pragma mark - Fields
Fields::~SyntaxList() = default;

Expression Fields::count() const
{
    return Expression::function("count").invokeAll();
}

ResultFields Fields::redirect(const ResultColumns& resultColumns) const
{
    ResultFields result;
    auto field = begin();
    auto resultColumn = resultColumns.begin();
    while (field != end() && resultColumn != resultColumns.end()) {
        result.push_back(ResultField(*resultColumn, field->getAccessor()));
        ++field;
        ++resultColumn;
    }
    return result;
}

ResultFields Fields::redirect(RedirectAction action) const
{
    WCTRemedialAssert(action != nullptr, "Redirect block can't be null.", return *this;);
    ResultFields results;
    for (const auto& field : *this) {
        results.push_back(ResultField(action(field), field.getAccessor()));
    }
    return results;
}

Expressions Fields::table(const StringView& table) const
{
    Expressions expressions;
    for (const auto& field : *this) {
        expressions.push_back(field.table(table));
    }
    return expressions;
}

Fields Fields::fieldsByAddingNewFields(const Fields& fields) const
{
    Fields newFields = *this;
    newFields.insert(newFields.begin(), fields.begin(), fields.end());
    return newFields;
}

Fields& Fields::addingNewFields(const Fields& fields)
{
    insert(end(), fields.begin(), fields.end());
    return *this;
}

bool Fields::isEqual(const Field& left, const Field& right)
{
    return left.getAccessor().get() == right.getAccessor().get()
           && left.getDescription() == right.getDescription();
}

Fields Fields::fieldsByRemovingFields(const Fields& fields) const
{
    Fields newFields;
    for (const auto& field : *this) {
        if (std::find_if(fields.begin(),
                         fields.end(),
                         std::bind(&Fields::isEqual, field, std::placeholders::_1))
            == fields.end()) {
            newFields.push_back(field);
        }
    }
    return newFields;
}

Fields& Fields::removingFields(const Fields& fields)
{
    for (const auto& field : fields) {
        auto iter = std::find_if(
        begin(), end(), std::bind(&Fields::isEqual, field, std::placeholders::_1));
        if (iter != end()) {
            erase(iter);
        }
    }
    return *this;
}

#pragma mark - Convertible

Expression ExpressionConvertible<Field>::asExpression(const Field& field)
{
    return Expression((const Column&) field);
}

IndexedColumn IndexedColumnConvertible<Field>::asIndexedColumn(const Field& field)
{
    return Expression((const Column&) field);
}

} // namespace WCDB
