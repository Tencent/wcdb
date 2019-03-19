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

#import <WCDB/Core.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResultColumn.h>

WCTResultColumn::WCTResultColumn()
: WCDB::ResultColumn()
, WCTColumnBindingHolder()
{
}

WCTResultColumn::WCTResultColumn(const WCTProperty& property)
: WCDB::ResultColumn(property)
, WCTColumnBindingHolder(property.getColumnBinding())
{
}

WCTResultColumn::WCTResultColumn(const WCDB::ResultColumn& resultColumn, const WCTColumnBinding& columnBinding)
: WCDB::ResultColumn(resultColumn)
, WCTColumnBindingHolder(columnBinding)
{
}

WCTResultColumns WCTResultColumns::resultColumnsByAddingNewResultColumns(const WCTResultColumns& resultColumns) const
{
    WCTResultColumns newResultColumns = *this;
    newResultColumns.insert(newResultColumns.end(), resultColumns.begin(), resultColumns.end());
    return newResultColumns;
}

WCTResultColumns& WCTResultColumns::addingNewResultColumns(const WCTResultColumns& resultColumns)
{
    insert(end(), resultColumns.begin(), resultColumns.end());
    return *this;
}

bool WCTResultColumns::isEqual(const WCTResultColumn& left, const WCTResultColumn& right)
{
    return left.getColumnBinding() == right.getColumnBinding() && left.getDescription() == right.getDescription();
}

WCTResultColumns WCTResultColumns::resultColumnsByRemovingResultColumns(const WCTResultColumns& resultColumns) const
{
    WCTResultColumns newResultColumns;
    for (const auto& resultColumn : *this) {
        if (std::find_if(resultColumns.begin(), resultColumns.end(), std::bind(&WCTResultColumns::isEqual, resultColumn, std::placeholders::_1)) == resultColumns.end()) {
            newResultColumns.push_back(resultColumn);
        }
    }
    return newResultColumns;
}

WCTResultColumns& WCTResultColumns::removingResultColumns(const WCTResultColumns& resultColumns)
{
    for (const auto& resultColumn : resultColumns) {
        auto iter = std::find_if(begin(), end(), std::bind(&WCTResultColumns::isEqual, resultColumn, std::placeholders::_1));
        if (iter != end()) {
            erase(iter);
        }
    }
    return *this;
}
