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

    MultiSelect &onResultFields(const ResultFields &resultFields);
    MultiSelect &fromTables(ValueArray<StringView> tableNames);
    MultiSelect &where(const Expression &condition);
    MultiSelect &orders(const OrderingTerms &orders);
    MultiSelect &limit(const Expression &limit);
    MultiSelect &offset(const Expression &offset);

    std::optional<MultiObject> firstMultiObject();
    std::optional<ValueArray<MultiObject>> allMultiObjects();

protected:
    MultiSelect(Recyclable<InnerDatabase *> databaseHolder);

private:
    bool prepareStatement();
    ResultFields m_fields;
};

} // namespace WCDB
