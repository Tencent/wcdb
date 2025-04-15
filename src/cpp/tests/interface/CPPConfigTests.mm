//
// Created by qiuwenchen on 2022/8/10.
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

#import "CPPTestCase.h"

@interface CPPConfigTests : CPPDatabaseTestCase

@property (nonatomic, readonly) NSString* configName;

@end

bool g_hasOpened = false;
bool g_hasClosed = false;

int newOpen(const char* path, int flag, int mode)
{
    g_hasOpened = true;
    return ::open(path, flag, mode);
}

int newClose(int fd)
{
    g_hasClosed = true;
    return ::close(fd);
}

@implementation CPPConfigTests {
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
    self.database->removeConfig(self.configName.UTF8String);
    [super tearDown];
}

- (void)test_open_close
{
    WCDB::Database::registerPOSIXOpen(newOpen);
    WCDB::Database::registerPOSIXClose(newClose);
    TestCaseAssertTrue(self.database->canOpen());
    self.database->close();
    TestCaseAssertTrue(g_hasOpened && g_hasClosed);
    WCDB::Database::registerPOSIXOpen(nullptr);
    WCDB::Database::registerPOSIXClose(nullptr);
    g_hasOpened = false;
    g_hasClosed = false;
    TestCaseAssertTrue(self.database->canOpen());
    self.database->close();
    TestCaseAssertTrue(!g_hasOpened && !g_hasClosed);
}

- (void)test_config
{
    WCDB::StatementPragma setSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(true);
    WCDB::StatementPragma unsetSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(false);
    WCDB::StatementPragma getSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete());

    BOOL uninvoked = NO;
    {
        self.database->setConfig(
        self.configName.UTF8String, [&](WCDB::Handle& handle) { return handle.execute(setSecureDelete); }, [&](WCDB::Handle& handle) {
            uninvoked = YES;
            return handle.execute(unsetSecureDelete); });

        self.expectMode = DatabaseTestCaseExpectSomeSQLs;
        [self doTestSQLs:@[ @"PRAGMA secure_delete = TRUE" ]
             inOperation:^BOOL {
                 self.database->close();
                 return self.database->canOpen();
             }];
        TestCaseAssertTrue(self.database->getValueFromStatement(getSecureDelete).value());
    }
    {
        // uninvocation
        self.database->removeConfig(self.configName.UTF8String);

        TestCaseAssertTrue(self.database->canOpen());
        TestCaseAssertTrue(uninvoked);

        auto value = self.database->getValueFromStatement(getSecureDelete);
        TestCaseAssertTrue(value.succeed());
        TestCaseAssertFalse(value.value());
    }
}

- (void)test_config_out_of_scope
{
    [self.dispatch async:^{
        WCDB::StatementPragma setSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(true);
        WCDB::Database db1(self.path.UTF8String);
        db1.setConfig(
        self.configName.UTF8String, [=](WCDB::Handle& handle) { return handle.execute(setSecureDelete); });
    }];
    [self.dispatch waitUntilDone];
    WCDB::Database db2(self.path.UTF8String);
    TestCaseAssertTrue(db2.canOpen());
}

- (void)test_config_failed
{
    self.database->setConfig(self.configName.UTF8String, [](WCDB::Handle&) {
        return false;
    });

    TestCaseAssertFalse(self.database->canOpen());
}

- (void)test_uninvoke_failed
{
    self.database->setConfig(
    self.configName.UTF8String, [](WCDB::Handle&) { return true; }, [](WCDB::Handle&) { return false; });

    TestCaseAssertTrue(self.database->canOpen());

    self.database->removeConfig(self.configName.UTF8String);

    TestCaseAssertFalse(self.database->canOpen());
}

- (void)test_ordered_config
{
    NSString* config1 = [self.configName stringByAppendingString:@"_1"];
    NSString* config2 = [self.configName stringByAppendingString:@"_2"];
    NSString* config3 = [self.configName stringByAppendingString:@"_3"];

    int step = 0;

    self.database->setConfig(config1.UTF8String, [&step](WCDB::Handle&) {
        ++step;
        return step == 2;
    });

    self.database->setConfig(
    config2.UTF8String, [&step](WCDB::Handle&) {
        ++step;
        return step == 3;
    },
    nil,
    WCDB::Database::Priority::Low);

    self.database->setConfig(
    config3.UTF8String, [&step](WCDB::Handle&) {
        ++step;
        return step == 1;
    },
    nil,
    WCDB::Database::Priority::High);

    TestCaseAssertTrue(self.database->canOpen());

    TestCaseAssertEqual(step, 3);
}

- (void)test_cipher
{
    NSData* cipher = Random.shared.data;
    NSData* wrongCipher = [Random.shared dataOtherThan:cipher];

    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) cipher.bytes, cipher.length));
    TestCaseAssertTrue(self.database->canOpen());

    self.database->close();
    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) wrongCipher.bytes, wrongCipher.length));
    TestCaseAssertFalse(self.database->canOpen());
}

- (void)test_cipher_with_page_size
{
    NSData* cipher = Random.shared.data;
    int pageSize = 8 * 1024;
    int wrongPageSize = 16 * 1024;

    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) cipher.bytes, cipher.length), pageSize);
    TestCaseAssertTrue(self.database->canOpen());

    self.database->close();
    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) cipher.bytes, cipher.length), wrongPageSize);
    TestCaseAssertFalse(self.database->canOpen());
}

- (void)test_cipher_with_diferent_version
{
    NSData* cipher = Random.shared.data;
    int pageSize = 4096;
    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) cipher.bytes, cipher.length), pageSize, WCDB::Database::CipherVersion::Version3);
    TestCaseAssertTrue(self.database->canOpen());

    self.database->close();
    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) cipher.bytes, cipher.length), pageSize);
    TestCaseAssertFalse(self.database->canOpen());

    WCDB::Database::setDefaultCipherConfiguration(WCDB::Database::CipherVersion::Version3);
    TestCaseAssertTrue(self.database->canOpen());

    self.database->removeFiles();
    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) cipher.bytes, cipher.length), pageSize, WCDB::Database::CipherVersion::Version4);
    TestCaseAssertTrue(self.database->canOpen());
    self.database->close();

    self.database->setCipherKey(WCDB::UnsafeData((unsigned char*) cipher.bytes, cipher.length), pageSize);
    TestCaseAssertFalse(self.database->canOpen());
    WCDB::Database::setDefaultCipherConfiguration(WCDB::Database::CipherVersion::Version4);
    TestCaseAssertTrue(self.database->canOpen());
}

- (void)testSetTemporaryDirectory
{
    TestCaseAssertFalse(WCDB::Database::setDefaultTemporaryDirectory("wrongDir"));

    NSString* tempDir = [NSTemporaryDirectory() stringByAppendingPathComponent:@"dbTempDir"];
    TestCaseAssertTrue(WCDB::Database::setDefaultTemporaryDirectory(tempDir.UTF8String));
    WCDB::StatementPragma getDirStatement = WCDB::StatementPragma().pragma(WCDB::Pragma("temp_store_directory"));
    WCDB::OptionalValue dir = self.database->getValueFromStatement(getDirStatement);
    TestCaseAssertTrue(dir.valueOr("").textValue().compare(tempDir.UTF8String) == 0);

    TestCaseAssertTrue(WCDB::Database::setDefaultTemporaryDirectory(NULL));
    dir = self.database->getValueFromStatement(getDirStatement);
    TestCaseAssertTrue(dir.hasValue() && dir.value().textValue().length() == 0);
}

@end
