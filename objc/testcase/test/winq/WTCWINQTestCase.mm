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

#import "WTCWINQTestCase.h"

@implementation WTCWINQTestCase

+ (std::string)schemaName
{
    return "testSchema";
}

+ (std::string)tableName
{
    return "testTable";
}

+ (std::string)indexName
{
    return "testIndex";
}

+ (std::string)triggerName
{
    return "testTrigger";
}

+ (std::string)viewName
{
    return "testView";
}

+ (std::string)columnName
{
    return "testColumn";
}

+ (std::string)columnName2
{
    return "testColumn2";
}

+ (std::list<std::string>)columnNames
{
    return {self.class.columnName, self.class.columnName2};
}

+ (std::string)collationName
{
    return "testCollation";
}

+ (std::string)savepointName
{
    return "testSavepoint";
}

+ (WCDB::StatementSelect)statementSelect
{
    WCDB::Expression column = WCDB::Expression::ColumnNamed(self.class.columnName);
    WCDB::ResultColumn resultColumn(column);
    WCDB::TableOrSubquery tableOrSubquery(self.class.tableName);
    return WCDB::StatementSelect()
        .select(resultColumn)
        .from(tableOrSubquery);
}

+ (WCDB::WithClause)withClause
{
    WCDB::CTETableName cteTableName(self.class.tableName);
    return WCDB::WithClause()
        .with(cteTableName, self.class.statementSelect);
}

+ (WCDB::ResultColumn)resultColumn
{
    WCDB::Expression column = WCDB::Expression::ColumnNamed(self.class.columnName);
    return WCDB::ResultColumn(column);
}

+ (std::list<WCDB::ResultColumn>)resultColumns
{
    std::list<WCDB::ResultColumn> resultColumns;
    for (const std::string &columnName : self.class.columnNames) {
        WCDB::Expression column = WCDB::Expression::ColumnNamed(columnName);
        resultColumns.push_back(column);
    }
    return resultColumns;
}

+ (WCDB::TableOrSubquery)tableOrSubquery
{
    return WCDB::TableOrSubquery(self.class.tableName);
}

+ (std::list<WCDB::TableOrSubquery>)tableOrSubquerys
{
    WCDB::TableOrSubquery tableOrSubquery1(self.class.tableName);
    WCDB::TableOrSubquery tableOrSubquery2("testTable2");
    return {tableOrSubquery1, tableOrSubquery2};
}

+ (WCDB::JoinClause)joinClause
{
    return WCDB::JoinClause(self.class.tableOrSubquery);
}

+ (WCDB::Expression)condition
{
    return WCDB::Expression(WCDB::Expression::ColumnNamed(self.class.columnName)).notNull();
}

+ (WCDB::Expression)group
{
    return WCDB::Expression::ColumnNamed(self.class.columnName);
}

+ (std::list<WCDB::Expression>)groups
{
    WCDB::Expression group1 = self.class.group;
    WCDB::Expression group2 = WCDB::Expression::ColumnNamed(self.class.columnName2);
    ;
    return {group1, group2};
}

+ (WCDB::Expression)having
{
    return self.class.condition;
}

+ (WCDB::Expression)value
{
    return WCDB::Expression(WCDB::LiteralValue(1));
}

+ (WCDB::Expression)value2
{
    return WCDB::Expression(WCDB::LiteralValue("testValue"));
}

+ (std::list<WCDB::Expression>)values
{
    return {self.class.value, self.class.value2};
}

+ (WCDB::OrderingTerm)orderingTerm
{
    return WCDB::OrderingTerm(WCDB::Expression::ColumnNamed(self.class.columnName));
}

+ (std::list<WCDB::OrderingTerm>)orderingTerms
{
    WCDB::OrderingTerm orderingTerm1 = self.class.orderingTerm;
    WCDB::OrderingTerm orderingTerm2 = WCDB::OrderingTerm(WCDB::Expression::ColumnNamed(self.class.columnName2));
    return {orderingTerm1, orderingTerm2};
}

+ (WCDB::Expression)limit
{
    return WCDB::Expression(WCDB::LiteralValue(1));
}

+ (WCDB::Expression)limitParameter
{
    return WCDB::Expression(WCDB::LiteralValue(2));
}

+ (WCDB::CommonTableExpression)commonTableExpression
{
    return WCDB::CommonTableExpression(self.class.tableName);
}

+ (std::list<WCDB::CommonTableExpression>)commonTableExpressions
{
    WCDB::CommonTableExpression commonTableExpression1 = self.class.commonTableExpression;
    WCDB::CommonTableExpression commonTableExpression2("testTable2");
    return {commonTableExpression1, commonTableExpression2};
}

+ (WCDB::QualifiedTableName)qualifiedTableName
{
    return WCDB::QualifiedTableName(self.class.tableName);
}

@end
