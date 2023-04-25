//
// Created by sanhuazhang on 2019/05/02
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

#import "TestCase.h"

@interface ConfigTests : DatabaseTestCase

@property (nonatomic, readonly) NSString* configName;

@end

@implementation ConfigTests {
    NSString* _configName;
}

- (NSString*)configName
{
    @synchronized(self) {
        if (_configName == nil) {
            _configName = Random.shared.string;
        }
        return _configName;
    }
}

- (void)tearDown
{
    [self.database removeConfigForName:self.configName];
    [super tearDown];
}

- (void)test_config
{
    WCDB::StatementPragma setSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(YES);
    WCDB::StatementPragma unsetSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(NO);
    WCDB::StatementPragma getSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete());

    __block BOOL uninvoked = NO;
    {
        // invocation
        [self.database
        setConfig:^BOOL(WCTHandle* handle) {
            return [handle execute:setSecureDelete];
        }
        withUninvocation:^BOOL(WCTHandle* handle) {
            // customlized check uninvocation since traceSQL will be unset in the test function
            uninvoked = YES;
            return [handle execute:unsetSecureDelete];
        }
        forName:self.configName];

        self.expectMode = DatabaseTestCaseExpectSomeSQLs;
        [self doTestSQLs:@[ @"PRAGMA secure_delete = TRUE" ]
             inOperation:^BOOL {
                 [self.database close];
                 return [self.database canOpen];
             }];

        TestCaseAssertTrue([self.database getValueFromStatement:getSecureDelete].numberValue.boolValue);
    }
    {
        // uninvocation
        [self.database removeConfigForName:self.configName];

        TestCaseAssertTrue([self.database canOpen]);
        TestCaseAssertTrue(uninvoked);

        TestCaseAssertFalse([self.database getValueFromStatement:getSecureDelete].numberValue.boolValue);
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
               WCDB_UNUSED(handle)
               ++step;
               return step == 2;
           }
    withUninvocation:nil
             forName:config1
        withPriority:WCTConfigPriorityDefault];

    [self.database
           setConfig:^BOOL(WCTHandle* handle) {
               WCDB_UNUSED(handle)
               ++step;
               return step == 3;
           }
    withUninvocation:nil
             forName:config2
        withPriority:WCTConfigPriorityLow];
    [self.database
           setConfig:^BOOL(WCTHandle* handle) {
               WCDB_UNUSED(handle)
               ++step;
               return step == 1;
           }
    withUninvocation:nil
             forName:config3
        withPriority:WCTConfigPriorityHigh];

    TestCaseAssertTrue([self.database canOpen]);

    TestCaseAssertEqual(step, 3);
}

- (void)test_cipher
{
    NSData* cipher = Random.shared.data;
    NSData* wrongCipher = [Random.shared dataOtherThan:cipher];

    [self.database setCipherKey:cipher];
    TestCaseAssertTrue([self.database canOpen]);

    [self.database close];
    [self.database setCipherKey:wrongCipher];
    TestCaseAssertFalse([self.database canOpen]);
}

- (void)test_cipher_with_page_size
{
    NSData* cipher = Random.shared.data;
    int pageSize = 8 * 1024;
    int wrongPageSize = 16 * 1024;

    [self.database setCipherKey:cipher andCipherPageSize:pageSize];
    TestCaseAssertTrue([self.database canOpen]);

    [self.database close];
    [self.database setCipherKey:cipher andCipherPageSize:wrongPageSize];
    TestCaseAssertFalse([self.database canOpen]);
}

- (void)test_cipher_with_diferent_version
{
    NSData* cipher = Random.shared.data;
    int pageSize = 4096;
    [self.database setCipherKey:cipher andCipherPageSize:pageSize andCipherViersion:WCTCipherVersion3];
    TestCaseAssertTrue([self.database canOpen]);

    [self.database close];
    [self.database setCipherKey:cipher andCipherPageSize:pageSize];
    TestCaseAssertFalse([self.database canOpen]);

    [WCTDatabase setDefaultCipherConfiguration:WCTCipherVersion3];
    TestCaseAssertTrue([self.database canOpen]);

    [self.database removeFiles];
    [self.database setCipherKey:cipher andCipherPageSize:pageSize andCipherViersion:WCTCipherVersion4];
    TestCaseAssertTrue([self.database canOpen]);
    [self.database close];

    [self.database setCipherKey:cipher andCipherPageSize:pageSize];
    TestCaseAssertFalse([self.database canOpen]);
    [WCTDatabase setDefaultCipherConfiguration:WCTCipherVersion4];
    TestCaseAssertTrue([self.database canOpen]);
}

- (void)test_abtest_config
{
    NSMutableDictionary* abtestDic = [[NSMutableDictionary alloc] init];
    for (int i = 0; i < 100; i++) {
        abtestDic[Random.shared.string] = Random.shared.string;
    }
    [abtestDic enumerateKeysAndObjectsUsingBlock:^(NSString* name, NSString* value, BOOL*) {
        [WCTDatabase setABTestConfigWithName:name andValue:value];
    }];

    [abtestDic enumerateKeysAndObjectsUsingBlock:^(NSString* name, NSString* value, BOOL*) {
        TestCaseAssertTrue([[WCTDatabase getABTestConfigWithName:name] isEqualToString:value]);
    }];

    [abtestDic enumerateKeysAndObjectsUsingBlock:^(NSString* name, NSString*, BOOL*) {
        [WCTDatabase removeABTestConfigWithName:name];
    }];

    [abtestDic enumerateKeysAndObjectsUsingBlock:^(NSString* name, NSString*, BOOL*) {
        TestCaseAssertTrue([WCTDatabase getABTestConfigWithName:name] == nil);
    }];
}

@end
