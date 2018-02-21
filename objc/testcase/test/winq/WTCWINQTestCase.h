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

#import "WTCAssert.h"
#import <WINQ/abstract.h>
#import <XCTest/XCTest.h>

@interface WTCWINQTestCase : XCTestCase

@property(class, readonly) std::string schemaName;
@property(class, readonly) std::string tableName;
@property(class, readonly) std::string indexName;
@property(class, readonly) std::string triggerName;
@property(class, readonly) std::string viewName;
@property(class, readonly) std::string columnName;
@property(class, readonly) std::list<std::string> columnNames;
@property(class, readonly) std::string collationName;
@property(class, readonly) std::string savepointName;

@property(class, readonly) WCDB::StatementSelect statementSelect;
@property(class, readonly) WCDB::WithClause withClause;
@property(class, readonly) WCDB::ResultColumn resultColumn;
@property(class, readonly) std::list<WCDB::ResultColumn> resultColumns;
@property(class, readonly) WCDB::TableOrSubquery tableOrSubquery;
@property(class, readonly) std::list<WCDB::TableOrSubquery> tableOrSubquerys;
@property(class, readonly) WCDB::JoinClause joinClause;
@property(class, readonly) WCDB::Expression condition;
@property(class, readonly) WCDB::Expression group;
@property(class, readonly) std::list<WCDB::Expression> groups;
@property(class, readonly) WCDB::Expression having;
@property(class, readonly) WCDB::Expression value;
@property(class, readonly) std::list<WCDB::Expression> values;
@property(class, readonly) WCDB::OrderingTerm orderingTerm;
@property(class, readonly) std::list<WCDB::OrderingTerm> orderingTerms;
@property(class, readonly) WCDB::Expression limit;
@property(class, readonly) WCDB::Expression limitParameter;
@property(class, readonly) WCDB::CommonTableExpression commonTableExpression;
@property(class, readonly) std::list<WCDB::CommonTableExpression>
    commonTableExpressions;

@end
