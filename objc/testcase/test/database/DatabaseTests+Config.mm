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

#import "TestCaseCommon.h"

@interface DatabaseTests_Config : TestCase

@end

@implementation DatabaseTests_Config {
    WCDB::StatementPragma _doSecureDelete;
    WCDB::StatementPragma _doNotSecureDelete;
    WCDB::StatementPragma _isSecureDelete;
}

- (void)setUp
{
    [super setUp];
    _isSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete());

    _doSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(YES);

    _doNotSecureDelete = WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(NO);
}

- (void)test_config_failed
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];

    [database setConfig:^BOOL(WCTHandle *_Nonnull) {
      return NO;
    }
                forName:@"failed"];
    XCTAssertFalse([database canOpen]);
}

- (void)test_config
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];
    {
        WCTValue *isSecureDelete = [database getValueFromStatement:_isSecureDelete];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertFalse(isSecureDelete.numberValue.boolValue);
    }
    {
        WCDB::StatementPragma doSecureDelete = _doSecureDelete;
        [database setConfig:^BOOL(WCTHandle *handle) {
          return [handle execute:doSecureDelete];
        }
                    forName:@"secure_delete"];
    }
    {
        WCTValue *isSecureDelete = [database getValueFromStatement:_isSecureDelete];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertTrue(isSecureDelete.numberValue.boolValue);
    }
}

- (void)test_ordered_config
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];
    __block int step = 0;
    [database setConfig:^BOOL(WCTHandle *handle) {
      ++step;
      return step == 1;
    }
                forName:@"step1"
              withOrder:10];
    [database setConfig:^BOOL(WCTHandle *handle) {
      ++step;
      return step == 2;
    }
                forName:@"step2"
              withOrder:10];
    [database setConfig:^BOOL(WCTHandle *handle) {
      ++step;
      return step == 3;
    }
                forName:@"step3"
              withOrder:20];
    XCTAssertTrue([database canOpen]);
    XCTAssertEqual(step, 3);
}

- (void)test_covered_config
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];
    __block BOOL step = NO;
    [database setConfig:^BOOL(WCTHandle *_Nonnull) {
      return NO;
    }
                forName:@"covered"];
    [database setConfig:^BOOL(WCTHandle *_Nonnull) {
      step = YES;
      return YES;
    }
                forName:@"covered"];
    XCTAssertTrue([database canOpen]);
    XCTAssertTrue(step);
}

- (void)test_reordered_config
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];
    __block int step = 0;
    [database setConfig:^BOOL(WCTHandle *handle) {
      ++step;
      return step == 2;
    }
                forName:@"step1"
              withOrder:20];
    [database setConfig:^BOOL(WCTHandle *handle) {
      return NO;
    }
                forName:@"step2"
              withOrder:30];
    [database setConfig:^BOOL(WCTHandle *handle) {
      ++step;
      return step == 1;
    }
                forName:@"step2"
              withOrder:10];
    XCTAssertTrue([database canOpen]);
    XCTAssertEqual(step, 2);
}

- (void)test_reconfig
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];
    {
        WCTValue *isSecureDelete = [database getValueFromStatement:_isSecureDelete];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertFalse(isSecureDelete.numberValue.boolValue);
    }
    {
        WCDB::StatementPragma doSecureDelete = _doSecureDelete;
        [database setConfig:^BOOL(WCTHandle *handle) {
          return [handle execute:doSecureDelete];
        }
                    forName:@"secure_delete"];
    }
    {
        WCTValue *isSecureDelete = [database getValueFromStatement:_isSecureDelete];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertTrue(isSecureDelete.numberValue.boolValue);
    }
    {
        WCDB::StatementPragma doNotSecureDelete = _doNotSecureDelete;
        [database setConfig:^BOOL(WCTHandle *handle) {
          return [handle execute:doNotSecureDelete];
        }
                    forName:@"secure_delete"];
    }
    {
        WCTValue *isSecureDelete = [database getValueFromStatement:_isSecureDelete];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertFalse(isSecureDelete.numberValue.boolValue);
    }
}

- (void)test_cipher
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];
    NSData *cipher = [NSData randomData];
    [database setCipherKey:cipher];
    XCTAssertTrue([database canOpen]);
    [database purge];
    NSData *wrongCipher = [NSData randomDataOtherThan:cipher];
    [database setCipherKey:wrongCipher];
    XCTAssertFalse([database canOpen]);
}

- (void)test_cipher_page_size
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:NSStringFromSelector(_cmd)]];
    NSData *cipher = [NSData randomData];
    [database setCipherKey:cipher andCipherPageSize:1024];
    XCTAssertTrue([database canOpen]);
    [database purge];
    [database setCipherKey:cipher andCipherPageSize:8192];
    XCTAssertFalse([database canOpen]);
}

@end
