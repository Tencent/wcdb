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

#import "SingleDatabaseTestCase.h"

@interface ConfigTests : SingleDatabaseTestCase

@property (nonatomic, readonly) NSString* configName;

@property (nonatomic, readonly) WCDB::StatementPragma setSecureDelete;
@property (nonatomic, readonly) WCDB::StatementPragma unsetSecureDelete;
@property (nonatomic, readonly) WCDB::StatementPragma getSecureDelete;

@end

@implementation ConfigTests

- (void)setUp
{
    [super setUp];
    _configName = [NSString randomString];

    _setSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(YES);
    _unsetSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(NO);
    _getSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete());
}

- (void)tearDown
{
    [self.database removeConfigForName:self.configName];
    [super tearDown];
}

- (void)test_config
{
    {
        [self.database
        setConfig:^BOOL(WCTHandle* handle) {
            return [handle execute:self.setSecureDelete];
        }
        withUninvocation:^BOOL(WCTHandle* handle) {
            return [handle execute:self.unsetSecureDelete];
        }
        forName:self.configName];

        BOOL result = [self checkSomeSQLs:@[ @"PRAGMA main.secure_delete = 1" ]
                    asExpectedByOperation:^BOOL {
                        [self.database close];
                        return [self.database canOpen];
                    }];
        TestCaseAssertTrue(result);

        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertTrue([handle prepare:self.getSecureDelete] && [handle step]);
        TestCaseAssertTrue([handle getNumberAtIndex:0].boolValue);
        [handle finalizeStatement];
    }
    {
        [self.database removeConfigForName:self.configName];

        BOOL result = [self checkSomeSQLs:@[ @"PRAGMA main.secure_delete = 0" ]
                    asExpectedByOperation:^BOOL {
                        [self.database close];
                        return [self.database canOpen];
                    }];
        TestCaseAssertTrue(result);

        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertTrue([handle prepare:self.getSecureDelete] && [handle step]);
        TestCaseAssertFalse([handle getNumberAtIndex:0].boolValue);
        [handle finalizeStatement];
    }
}

@end
