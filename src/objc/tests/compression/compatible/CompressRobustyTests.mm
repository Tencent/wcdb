//
// Created by qiuwenchen on 2023/12/18.
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

@interface CompressRobustyTests : RobustyTestCase

@end

@implementation CompressRobustyTests

- (void)setUp
{
    [super setUp];
    self.skipDebugLog = YES;
    [self.database enableAutoCheckpoint:YES];
    self.tableFactor = 10;
}

- (NSString*)tablePrefix
{
    return @"t_";
}

- (void)do_test_feature_page_based_robusty
{
    NSArray* sizes = @[
        @(10 * 1024 * 1024),
#ifndef WCDB_QUICK_TESTS
        @(100 * 1024 * 1024),
        @(1024 * 1024 * 1024),
#endif
    ];
    for (NSNumber* size in sizes) {
        self.expectedDatabaseSize = size.integerValue;
        [self.database setCompressionWithFilter:nil];
        [self.database removeFiles];
        TestCaseAssertTrue([self fillDatabaseUntilMeetExpectedSize]);

        [self.database truncateCheckpoint];

        NSString* testPath = [self.database.path stringByAppendingString:@"-test"];
        WCTDatabase* database = [[WCTDatabase alloc] initWithPath:testPath];
        [database removeFiles];
        [self.fileManager copyItemsIfExistsAtPath:self.database.path toPath:testPath];
        [database setCompressionWithFilter:^(WCTCompressionUserInfo* info) {
            if ([info.table hasPrefix:[self tablePrefix]]) {
                [info addZSTDNormalCompressProperty:TestCaseObject.content];
            }
        }];
        for (int i = 0; i < 100; i++) {
            [self.dispatch async:^{
                for (int j = 0; j * 100 + i < self.allTables.count; j++) {
                    NSString* tableName = [self.allTables objectAtIndex:j * 100 + i];
                    TestCaseObject* object = [database getObjectOfClass:TestCaseObject.class fromTable:tableName];
                    TestCaseAssertNotNil(object);
                }
            }];
        }
        [self.dispatch waitUntilDone];
    }
}

@end
