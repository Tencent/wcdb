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

#include <WCDB/WINQ.h>

StatementInsert& StatementInsert::with(const WithClause& withClause)
{
    LangInsertSTMT& lang = getMutableLang();
    lang.withClause.assign(withClause.getLang());
    return *this;
}

StatementInsert& StatementInsert::insertInto(const std::string& tableName)
{
    insertInto(tableName, LangInsertSTMT::Type::Insert);
    return *this;
}

StatementInsert& StatementInsert::insertOrReplaceInto(const std::string& tableName)
{
    insertInto(tableName, LangInsertSTMT::Type::InsertOrReplace);
    return *this;
}

StatementInsert& StatementInsert::insertOrRollbackInto(const std::string& tableName)
{
    insertInto(tableName, LangInsertSTMT::Type::InsertOrRollback);
    return *this;
}

StatementInsert& StatementInsert::insertOrAbortInto(const std::string& tableName)
{
    insertInto(tableName, LangInsertSTMT::Type::InsertOrAbort);
    return *this;
}

StatementInsert& StatementInsert::insertOrFailInto(const std::string& tableName)
{
    insertInto(tableName, LangInsertSTMT::Type::InsertOrFail);
    return *this;
}

StatementInsert& StatementInsert::insertOrIgnoreInto(const std::string& tableName)
{
    insertInto(tableName, LangInsertSTMT::Type::InsertOrIgnore);
    return *this;
}

StatementInsert& StatementInsert::withSchema(const std::string& schemaName)
{
    LangInsertSTMT& lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementInsert& StatementInsert::on(const std::string& columnName)
{
    LangInsertSTMT& lang = getMutableLang();
    lang.columnNames.append(columnName);
    return *this;
}

StatementInsert& StatementInsert::on(const std::list<std::string>& columnNames)
{
    LangInsertSTMT& lang = getMutableLang();
    for (const std::string& columnName : columnNames) {
        lang.columnNames.append(columnName);
    }
    return *this;
}

StatementInsert& StatementInsert::values(const Expression& value)
{
    LangInsertSTMT& lang = getMutableLang();
    lang.switcher = LangInsertSTMT::Switch::Values;
    lang.exprs.append(value.getLang());
    return *this;
}

StatementInsert& StatementInsert::values(const std::list<Expression>& values)
{
    LangInsertSTMT& lang = getMutableLang();
    lang.switcher = LangInsertSTMT::Switch::Values;
    for (const Expression& value : values) {
        lang.exprs.append(value.getLang());
    }
    return *this;
}

StatementInsert& StatementInsert::values(const StatementSelect& selectSTMT)
{
    LangInsertSTMT& lang = getMutableLang();
    lang.switcher = LangInsertSTMT::Switch::Select;
    lang.selectSTMT.assign(selectSTMT.getLang());
    return *this;
}

StatementInsert& StatementInsert::defaultValues()
{
    LangInsertSTMT& lang = getMutableLang();
    lang.switcher = LangInsertSTMT::Switch::DefaultValues;
    return *this;
}

Statement::Type StatementInsert::getType() const
{
    return Statement::Type::Insert;
}

void StatementInsert::insertInto(const std::string& tableName, const LangInsertSTMT::Type& type)
{    
    LangInsertSTMT& lang = getMutableLang();
    lang.type = type; 
    lang.tableName.assign(tableName);   
}
