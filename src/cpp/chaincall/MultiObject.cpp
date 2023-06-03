//
// Created by qiuwenchen on 2022/10/24.
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

#include "MultiObject.hpp"
#include "Assertion.hpp"

namespace WCDB {

MultiObject::MultiObject() = default;

MultiObject::~MultiObject() = default;

bool MultiObject::operator==(const MultiObject& other) const
{
    if (m_objs.size() != other.m_objs.size()) {
        return false;
    }
    for (auto iter = m_objs.begin(); iter != m_objs.end(); iter++) {
        auto otherIter = other.m_objs.find(iter->first);
        if (otherIter == other.m_objs.end()) {
            return false;
        }
        if (iter->second != otherIter->second) {
            return false;
        }
    }
    return true;
}

bool MultiObject::operator!=(const MultiObject& other) const
{
    return !(operator==(other));
}

bool MultiObject::ObjectValue::operator==(const ObjectValue& other) const
{
    StringViewSet checkedFields;
    for (auto iter = m_fields.begin(); iter != m_fields.end(); iter++) {
        const StringView& columnName
        = iter->syntax().expression.getOrCreate().column().name;
        checkedFields.insert(columnName);
        const Value& myValue = m_values.at(columnName);
        auto otherIter = other.m_values.find(columnName);
        if (otherIter == other.m_values.end()) {
            if (!myValue.isEmpty()) {
                return false;
            }
        } else if (myValue != otherIter->second) {
            return false;
        }
    }
    for (auto iter = other.m_fields.begin(); iter != other.m_fields.end(); iter++) {
        const StringView& columnName
        = iter->syntax().expression.getOrCreate().column().name;
        if (checkedFields.find(columnName) != checkedFields.end()) {
            continue;
        }
        const Value& otherValue = other.m_values.at(columnName);
        if (!otherValue.isEmpty()) {
            return false;
        }
    }
    return true;
}

bool MultiObject::ObjectValue::operator!=(const ObjectValue& other) const
{
    return !(operator==(other));
}

void MultiObject::addField(const UnsafeStringView& table, const ResultField& field, const Value& value)
{
    WCTAssert(table.length() > 0);
    auto iter = m_objs.find(table);
    if (iter == m_objs.end()) {
        ObjectValue obj;
        obj.addField(field, value);
        m_objs[table] = obj;
    } else {
        iter->second.addField(field, value);
    }
}

void MultiObject::ObjectValue::addField(const ResultField& field, const Value& value)
{
    const StringView& columnName
    = field.syntax().expression.getOrCreate().column().name;
    WCTRemedialAssert(
    columnName.length() > 0,
    StringView::formatted("invalid result column %s", field.getDescription().data()),
    return;);
    WCTAssert(m_values.find(columnName) == m_values.end());
    m_fields.push_back(field);
    m_values[columnName] = value;
}

} // namespace WCDB
