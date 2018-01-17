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
#import "WTCBaseTestCase.h"

@interface WTCStatementCreateVirtualTableTests : WTCBaseTestCase

@end

@implementation WTCStatementCreateVirtualTableTests

- (void)testStatementCreateVirtualTable
{
    //Give
    WCDB::ModuleArgument moduleArgument1("left", "right");
    WCDB::ModuleArgument moduleArgument2 = WCDB::ModuleArgument::Tokenize("WCDB");
    
    std::string module1 = "module1";
    
    std::string virtualTable1 = "virtualTable1";
    
    //Then
    XCTAssertEqual(WCDB::StatementCreateVirtualTable().getStatementType(), WCDB::Statement::Type::CreateVirtualTable);
    
    WINQAssertEqual(WCDB::StatementCreateVirtualTable().create(virtualTable1).usingModule(module1, {moduleArgument1, moduleArgument2}), @"CREATE VIRTUAL TABLE IF NOT EXISTS virtualTable1 USING module1(left=right, tokenize=WCDB)");
    
    WINQAssertEqual(WCDB::StatementCreateVirtualTable().create(virtualTable1, false).usingModule(module1, {moduleArgument1, moduleArgument2}), @"CREATE VIRTUAL TABLE virtualTable1 USING module1(left=right, tokenize=WCDB)");
}

@end
