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

#ifndef LangCreateTriggerSTMT_hpp
#define LangCreateTriggerSTMT_hpp

#include <WCDB/lang_common.h>

namespace WCDB {

namespace lang {

class CreateTriggerSTMT : public Lang {
public:
    bool temp;
    bool ifNotExists;
    copy_on_write_string schemaName;
    copy_on_write_string triggerName;
    enum class Type : int {
        After,
        Before,
        InsteadOf,
    };
    Type type;

    enum class Operation : int {
        Delete,
        Insert,
        Update,
    };
    Operation operation;
    copy_on_write_lazy_string_list columnNames;

    copy_on_write_string tableName;

    bool forEachRow;
    copy_on_write_lazy_lang<Expr> expr;

    copy_on_write_lazy_lang_list<CRUDLang> STMTs;

    virtual copy_on_write_string SQL() const override;

protected:
    constexpr static const char *TypeName(const Type &type);
    constexpr static const char *OperationName(const Operation &operation);
};

} // namespace lang

} // namespace WCDB

#endif /* LangCreateTriggerSTMT_hpp */
