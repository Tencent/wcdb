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

#import "WINQTestCase.h"

@implementation WINQTestCase

- (void)setUp
{
    [super setUp];
    self.continueAfterFailure = YES;
}

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

+ (std::string)collationName
{
    return "testCollation";
}

+ (std::string)savepointName
{
    return "testSavepoint";
}

+ (std::string)constraintName
{
    return "testConstraint";
}

+ (std::string)functionName
{
    return "testFunction";
}

+ (WCDB::Column)column
{
    return WCDB::Column("testColumn");
}

+ (WCDB::Column)column2
{
    return WCDB::Column("testColumn2");
}

+ (std::list<WCDB::Column>)columns
{
    return {self.class.column, self.class.column2};
}

+ (WCDB::StatementUpdate)statementUpdate
{
    return WCDB::StatementUpdate()
        .update(self.class.qualifiedTableName)
        .set(self.class.column, self.class.value);
}

+ (WCDB::StatementInsert)statementInsert
{
    return WCDB::StatementInsert()
        .insertInto(self.class.tableName)
        .values(self.class.value);
}

+ (WCDB::StatementDelete)statementDelete
{
    return WCDB::StatementDelete()
        .deleteFrom(self.class.qualifiedTableName);
}

+ (WCDB::StatementSelect)statementSelect
{
    return WCDB::StatementSelect()
        .select(self.class.resultColumn)
        .from(self.class.tableOrSubquery);
}

+ (WCDB::WithClause)withClause
{
    WCDB::CTETableName cteTableName(self.class.tableName);
    return WCDB::WithClause()
        .with(cteTableName, self.class.statementSelect);
}

+ (WCDB::ResultColumn)resultColumn
{
    WCDB::Expression column = WCDB::Expression(self.class.column);
    return WCDB::ResultColumn(column);
}

+ (std::list<WCDB::ResultColumn>)resultColumns
{
    std::list<WCDB::ResultColumn> resultColumns;
    for (const WCDB::Column &column : self.class.columns) {
        resultColumns.push_back(WCDB::Expression(column));
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
    return WCDB::Expression(self.class.column).notNull();
}

+ (WCDB::Expression)group
{
    return WCDB::Expression(self.class.column);
}

+ (std::list<WCDB::Expression>)groups
{
    WCDB::Expression group1 = self.class.group;
    WCDB::Expression group2 = WCDB::Expression(self.class.column2);
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
    return WCDB::OrderingTerm(WCDB::Expression(self.class.column));
}

+ (std::list<WCDB::OrderingTerm>)orderingTerms
{
    WCDB::OrderingTerm orderingTerm1 = self.class.orderingTerm;
    WCDB::OrderingTerm orderingTerm2 = WCDB::OrderingTerm(WCDB::Expression(self.class.column2));
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
    return WCDB::CommonTableExpression(self.class.tableName).as(self.class.statementSelect);
}

+ (std::list<WCDB::CommonTableExpression>)commonTableExpressions
{
    WCDB::CommonTableExpression commonTableExpression1 = self.class.commonTableExpression;
    WCDB::CommonTableExpression commonTableExpression2 = WCDB::CommonTableExpression("testTable2").as(self.class.statementSelect);
    return {commonTableExpression1, commonTableExpression2};
}

+ (WCDB::QualifiedTableName)qualifiedTableName
{
    return WCDB::QualifiedTableName(self.class.tableName);
}

+ (WCDB::ForeignKeyClause)foreignKeyClause
{
    return WCDB::ForeignKeyClause("testForeignTable");
}

+ (WCDB::LiteralValue)literalValue
{
    return WCDB::LiteralValue(1);
}

+ (WCDB::ColumnConstraint)columnConstraint
{
    return WCDB::ColumnConstraint().withNotNull();
}

+ (std::list<WCDB::ColumnConstraint>)columnConstraints
{
    return {
        WCDB::ColumnConstraint().withNotNull(),
        WCDB::ColumnConstraint().withUnique(),
    };
}

+ (WCDB::ColumnDef)columnDef
{
    return WCDB::ColumnDef(self.class.column).withType(WCDB::ColumnType::Integer32);
}

+ (std::list<WCDB::ColumnDef>)columnDefs
{
    return {
        self.class.columnDef,
        WCDB::ColumnDef(self.class.column2)
            .withType(WCDB::ColumnType::Text),
    };
}

+ (WCDB::TableConstraint)tableConstraint
{
    return WCDB::TableConstraint(self.class.constraintName).withPrimaryKey(self.class.indexedColumn);
}

+ (std::list<WCDB::TableConstraint>)tableConstraints
{
    return {
        self.class.tableConstraint,
        WCDB::TableConstraint("testConstraint2")
            .withUnique(WCDB::IndexedColumn(self.class.column2)),
    };
}

+ (WCDB::IndexedColumn)indexedColumn
{
    return WCDB::IndexedColumn(self.class.column);
}

+ (std::list<WCDB::IndexedColumn>)indexedColumns
{
    std::list<WCDB::IndexedColumn> indexedColumns;
    for (const WCDB::Column &column : self.class.columns) {
        indexedColumns.push_back(WCDB::IndexedColumn(column));
    }
    return indexedColumns;
}

+ (WCDB::ModuleArgument)moduleArgument
{
    return WCDB::ModuleArgument("tokenize", "WCDB");
}

+ (std::list<WCDB::ModuleArgument>)moduleArguments
{
    return {
        self.class.moduleArgument,
        WCDB::ModuleArgument(self.class.columnDef),
    };
}

+ (WCDB::CTETableName)cteTableName
{
    return WCDB::CTETableName(self.class.tableName);
}

@end
