//
// Created by qiuwenchen on 2023/12/10.
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

#import "CompressionBenchmark.h"
#import <Foundation/Foundation.h>

static WCTDictId kDictId = 1;

@implementation CompressionBenchmark

- (void)setUp
{
    [super setUp];
    self.testQuality = 10000;
    switch (self.mode) {
    case CompressionMode_NoCompression:
        self.factory.category = @"NoCompression";
        break;
    case CompressionMode_NormalCompress:
        self.factory.category = @"NormalCompress";
        break;
    case CompressionMode_DictCompress:
        self.factory.category = @"DictCompress";
        break;
    }
    [[Random shared] setStringType:RandomStringType_English];
    [self configCompression:self.factory.database];

    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSMutableArray *contents = [[NSMutableArray alloc] init];
        for (int i = 0; i < 3000; i++) {
            [contents addObject:[[Random shared] string]];
        }
        NSData *dict = [WCTDatabase trainDictWithStrings:contents andDictId:kDictId];
        XCTAssertNotNil(dict);
        TestCaseAssertTrue([WCTDatabase registerZSTDDict:dict andDictId:kDictId]);
    });
}

- (void)tearDown
{
    [super tearDown];
    [[Random shared] setStringType:RandomStringType_Default];
}

- (void)setUpDatabase
{
    [super setUpDatabase];
    [self configCompression:self.database];
}

- (void)configCompression:(WCTDatabase *)database
{
    switch (self.mode) {
    case CompressionMode_NormalCompress: {
        [database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName]) {
                return;
            }
            [info addZSTDNormalCompressProperty:TestCaseObject.content];
        }];
    } break;
    case CompressionMode_DictCompress: {
        [database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName]) {
                return;
            }
            [info addZSTDDictCompressProperty:TestCaseObject.content withDictId:kDictId];
        }];
    } break;
    default:
        break;
    }

    if (self.mode == CompressionMode_NoCompression) {
        return;
    }
    TestCaseAssertFalse([database isCompressed]);
}

- (void)test_read
{
    [self doTestRead];
}

- (void)test_batch_read
{
    [self doTestBatchRead];
}

- (void)test_write
{
    [self doTestWrite];
}

- (void)test_batch_write
{
    [self doTestBatchWrite];
}

- (void)test_random_read
{
    [self doTestRandomRead];
}

- (void)test_random_update
{
    [self doTestRandomUpdate];
}

- (void)test_random_delete
{
    [self doTestRandomDelete];
}

@end
