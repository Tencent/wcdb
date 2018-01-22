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

#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTValue.h>
#import <WCDB/error.hpp>

bool WCTColumnBinding::isPrimary() const
{
    return m_isPrimary;
}

bool WCTColumnBinding::isAutoIncrement() const
{
    return m_isAutoIncrement;
}

const WCDB::ColumnDef WCTColumnBinding::getColumnDef() const
{
    return m_columnDef;
}

void WCTColumnBinding::makePrimary(WCTOrderTerm order, bool isAutoIncrement, WCTConflict conflict)
{
    m_isPrimary = true;
    m_isAutoIncrement = isAutoIncrement;
    m_columnDef.makePrimary((WCDB::OrderTerm) order, isAutoIncrement, (WCDB::Conflict) conflict);
}

void WCTColumnBinding::makeNotNull()
{
    m_columnDef.makeNotNull();
}

void WCTColumnBinding::makeUnique()
{
    m_columnDef.makeUnique();
}

void WCTColumnBinding::makeDefault(const WCDB::Expression defaultValue)
{
    m_columnDef.makeDefault(defaultValue);
}

void WCTColumnBinding::makeDefault(WCTDefaultType defaultType)
{
    m_columnDef.makeDefault((WCDB::ColumnDef::DefaultType) defaultType);
}

Class WCTColumnBinding::getClass() const
{
    return m_class;
}
