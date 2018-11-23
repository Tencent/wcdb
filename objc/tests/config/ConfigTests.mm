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
    [self removeSQLRelatedConfigs];

    __block BOOL uninvoked = NO;
    {
        // invocation
        [self.database
        setConfig:^BOOL(WCTHandle* handle) {
            return [handle execute:self.setSecureDelete];
        }
        withUninvocation:^BOOL(WCTHandle* handle) {
            // customlized check uninvocation since traceSQL will be unset in the test function
            uninvoked = YES;
            return [handle execute:self.unsetSecureDelete];
        }
        forName:self.configName];

        BOOL result = [self checkAllSQLs:@[ @"PRAGMA main.secure_delete = 1" ]
                   asExpectedInOperation:^BOOL {
                       [self.database close];
                       return [self.database canOpen];
                   }];
        TestCaseAssertTrue(result);

        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertTrue([handle prepare:self.getSecureDelete] && [handle step]);
        TestCaseAssertTrue([handle getNumberAtIndex:0].boolValue);
        [handle finalizeStatement];
        [handle invalidate];
    }
    {
        // uninvocation
        [self.database removeConfigForName:self.configName];

        TestCaseAssertTrue([self.database canOpen]);
        TestCaseAssertTrue(uninvoked);

        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertTrue([handle prepare:self.getSecureDelete] && [handle step]);
        TestCaseAssertFalse([handle getNumberAtIndex:0].boolValue);
        [handle finalizeStatement];
        [handle invalidate];
    }
}

- (void)test_config_failed
{
    [self.database
           setConfig:^BOOL(WCTHandle* _Nonnull) {
               return NO;
           }
    withUninvocation:nil
             forName:self.configName];

    TestCaseAssertFalse([self.database canOpen]);
}

- (void)test_uninvoke_failed
{
    [self.database
    setConfig:^BOOL(WCTHandle* _Nonnull) {
        return YES;
    }
    withUninvocation:^BOOL(WCTHandle* _Nonnull) {
        return NO;
    }
    forName:self.configName];

    TestCaseAssertTrue([self.database canOpen]);

    [self.database removeConfigForName:self.configName];

    TestCaseAssertFalse([self.database canOpen]);
}

- (void)test_ordered_config
{
    NSString* config1 = [self.configName stringByAppendingString:@"_1"];
    NSString* config2 = [self.configName stringByAppendingString:@"_2"];
    NSString* config3 = [self.configName stringByAppendingString:@"_3"];

    __block int step = 0;

    [self.database
           setConfig:^BOOL(WCTHandle* handle) {
               ++step;
               return step == 2;
           }
    withUninvocation:nil
             forName:config1
        withPriority:WCTConfigPriorityDefault];

    [self.database
           setConfig:^BOOL(WCTHandle* handle) {
               ++step;
               return step == 3;
           }
    withUninvocation:nil
             forName:config2
        withPriority:WCTConfigPriorityLow];
    [self.database
           setConfig:^BOOL(WCTHandle* handle) {
               ++step;
               return step == 1;
           }
    withUninvocation:nil
             forName:config3
        withPriority:WCTConfigPriorityHigh];

    XCTAssertTrue([self.database canOpen]);

    XCTAssertEqual(step, 3);
}

- (void)test_cipher
{
    NSData* cipher = [NSData randomData];
    NSData* wrongCipher = [NSData randomDataOtherThan:cipher];

    [self.database setCipherKey:cipher];
    TestCaseAssertTrue([self.database canOpen]);

    [self.database close];
    [self.database setCipherKey:wrongCipher];
    TestCaseAssertFalse([self.database canOpen]);
}

- (void)test_cipher_with_page_size
{
    NSData* cipher = [NSData randomData];
    int pageSize = 8 * 1024;
    int wrongPageSize = 16 * 1024;

    [self.database setCipherKey:cipher andCipherPageSize:pageSize];
    TestCaseAssertTrue([self.database canOpen]);

    [self.database close];
    [self.database setCipherKey:cipher andCipherPageSize:wrongPageSize];
    TestCaseAssertFalse([self.database canOpen]);
}

@end
