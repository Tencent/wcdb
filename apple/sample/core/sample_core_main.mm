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

#import "sample_core_main.h"
#import "WCTSampleCore+WCTTableCoding.h"
#import "WCTSampleCore.h"
#import "WCTSampleCoreExt+WCTTableCoding.h"
#import "WCTSampleCoreExt.h"
#import <WCDB/WCDB.h>

void sample_core_main(NSString *baseDirectory)
{
    NSLog(@"Sample-core Begin");
    NSString *path = [baseDirectory stringByAppendingPathComponent:@"core"];
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];

    //set config
    [database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> handle, WCDB::Error &error) {
      return handle->exec(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete, YES));
    }
                forName:@"demo"
              withOrder:1];

    //run unwrapped SQL
    [database exec:WCDB::StatementPragma().pragma(WCDB::Pragma::CaseSensitiveLike, true)];

    //get value from unwrapped SQL
    {
        WCTStatement *statement = [database prepare:WCDB::StatementPragma().pragma(WCDB::Pragma::CacheSize)];
        if (statement && [statement step]) {
            NSLog(@"Cache size %@", [statement getValueAtIndex:0]);
        }
    }

    //complex statement 1
    //SAVEPOINT mySavepoint
    [database exec:WCDB::StatementSavepoint().savepoint("mySavepoint")];
    //...
    //RELEASE SAVEPOINT mySavepoint
    [database exec:WCDB::StatementRelease().release("mySavepoint")];

    //complex statement 2
    //EXPLAIN CREATE TABLE message(localID INTEGER PRIMARY KEY ASC, content TEXT);
    {
        NSLog(@"Explain:");
        //Column def for localID
        WCDB::ColumnDef localIDColumnDef = WCDB::ColumnDef(WCTSampleCore.localID, WCDB::ColumnType::Integer32).makePrimary(WCDB::OrderTerm::ASC);

        //Column def for content
        WCDB::ColumnDef contentColumnDef(WCTSampleCore.content, WCDB::ColumnType::Text);

        //Combine them into column def list
        WCDB::ColumnDefList columnDefList = {localIDColumnDef, contentColumnDef};

        //Combine table name and column def list into create table statement
        WCDB::StatementCreateTable statementCreate = WCDB::StatementCreateTable().create("message", columnDefList);

        WCTStatement *statementExplain = [database prepare:WCDB::StatementExplain().explain(statementCreate)];
        if (statementExplain && [statementExplain step]) {
            for (int i = 0; i < [statementExplain getColumnCount]; ++i) {
                NSString *columnName = [statementExplain getColumnNameAtIndex:i];
                WCTValue *value = [statementExplain getValueAtIndex:i];
                NSLog(@"%@:%@", columnName, value);
            }
        }
    }

    //complex statement 3
    //SELECT message.content, message_ext.createTime FROM message LEFT OUTER JOIN message_ext ON message.localID=message_ext.localID
    {
        //Column result list
        WCTResultList resultList = { WCTSampleCore.content.inTable(@"message"),
                                     WCTSampleCoreExt.createTime.inTable(@"message_ext") };

        //Join clause
        WCDB::JoinClause joinClause = WCDB::JoinClause("message").join("message_ext", WCDB::JoinClause::Type::LeftOuter).on(WCTSampleCore.localID.inTable(@"message") == WCTSampleCoreExt.localID.inTable(@"message_ext"));

        WCDB::StatementSelect statementSelect = WCDB::StatementSelect().select(resultList).from(joinClause);
        WCTError *error;
        WCTStatement *statement = [database prepare:statementSelect withError:&error];
        if (statement) {
            while ([statement step]) {
                for (int i = 0; i < [statement getColumnCount]; ++i) {
                    NSString *columnName = [statement getColumnNameAtIndex:i];
                    WCTValue *value = [statement getValueAtIndex:i];
                    NSLog(@"%@:%@", columnName, value);
                }
            }
            error = [statement getError];
            if (error) {
                NSLog(@"Error %@", error);
            }
        } else {
            NSLog(@"Error %@", error);
        }
    }

    NSLog(@"Sample-core End");
}
