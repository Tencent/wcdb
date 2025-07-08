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

#include "Binding.hpp"
#include "Accessor.hpp"
#include "Assertion.hpp"
#include "Field.hpp"

namespace WCDB {

Binding::Binding() : BaseBinding() {};

Binding::~Binding() {};

#pragma mark - Inherit
void Binding::inherit(const Binding &binding)
{
    m_columnDefs.insert(
    m_columnDefs.end(), binding.m_columnDefs.begin(), binding.m_columnDefs.end());
    m_constraints.insert(binding.m_constraints.begin(), binding.m_constraints.end());
    m_indexes.insert(binding.m_indexes.begin(), binding.m_indexes.end());
    m_fields.insert(m_fields.end(), binding.m_fields.begin(), binding.m_fields.end());
}

#pragma mark - Field

const Field &
Binding::registerField(void *memberPointer, const UnsafeStringView &name, BaseAccessor *accessor)
{
    m_fieldMap[memberPointer] = std::make_pair(name, accessor);
    m_columnDefs.emplace_back(
    std::make_pair(name, ColumnDef(name, accessor->getColumnType())));
    m_fields.emplace_back(name, accessor);
    m_fields.back().syntax().tableBinding = this;
    return m_fields.back();
}

const StringView &Binding::getColumnName(void *memberPointer) const
{
    auto iter = m_fieldMap.find(memberPointer);
    WCTAssert(iter != m_fieldMap.end());
    return iter->second.first;
}

const BaseAccessor *Binding::getAccessor(void *memberPointer) const
{
    auto iter = m_fieldMap.find(memberPointer);
    WCTAssert(iter != m_fieldMap.end());
    return iter->second.second;
}

const Fields &Binding::allFields() const
{
    return m_fields;
}

#pragma mark - ColumnDef
ColumnDef *Binding::getColumnDef(void *memberPointer)
{
    const StringView &name = getColumnName(memberPointer);
    return BaseBinding::getColumnDef(name);
}

#pragma mark - IndexColumn
IndexedColumn Binding::getIndexColumn(void *memberPointer)
{
    const StringView &name = getColumnName(memberPointer);
    return IndexedColumn(name);
}

} //namespace WCDB
