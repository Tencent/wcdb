//
// Created by 陈秋文 on 2022/10/25.
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

#include <WCDB/ChainCall.hpp>
#include <WCDB/MultiObject.hpp>

namespace WCDB {

class MultiSelect : public ChainCall<StatementSelect> {
    template<class ObjectType>
    friend class TableORMOperation;
    friend class HandleORMOperation;

public:
    ~MultiSelect();

    /**
     @brief WINQ interface for SQL.
     @param condition condition
     @return this
     */
    MultiSelect &where(const Expression &condition);

    /**
     @brief WINQ interface for SQL.
     @param orders order list
     @return this
     */
    MultiSelect &orders(const OrderingTerms &orders);

    /**
     @brief WINQ interface for SQL.
     @param limit limit
     @return this
     */
    MultiSelect &limit(const Expression &limit);

    /**
     @brief WINQ interface for SQL.
     @param offset offset
     @return this
     */
    MultiSelect &offset(const Expression &offset);

    /**
     @brief WINQ interface for SQL.
     @param resultFields The column results to be selected.
     @return this
     */
    MultiSelect &onResultFields(const ResultFields &resultFields);

    /**
     @brief WINQ interface for SQL.
     @param tableNames The names of the tables to query data from.
     @return this
     */
    MultiSelect &fromTables(ValueArray<StringView> tableNames);

    /**
     @brief Get first selected objects.
     
         Optional<WCDB::MultiObject> multiObject = multiSelect.firstMultiObject();
         if(multiObject.succeed()) {
             TestClass1 object1 = multiObject.value().objectAtTable<TestClass1>("table1");
             TestClass2 object2 = multiObject.value().objectAtTable<TestClass2>("table2");
         }

     @return See the example above
     */
    Optional<MultiObject> firstMultiObject();

    /**
     @brief Get all selected objects.
     @return Array with all selected objects.
     */
    Optional<ValueArray<MultiObject>> allMultiObjects();

protected:
    MultiSelect(Recyclable<InnerDatabase *> databaseHolder);

private:
    bool prepareStatement();
    ResultFields m_fields;
};

} // namespace WCDB
