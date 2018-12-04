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

@implementation SingleDatabaseTestCase

- (void)setUp
{
    [super setUp];
    [self refreshDirectory];
    _path = [self.directory stringByAppendingPathComponent:@"testDatabase"];
    _walPath = [_path stringByAppendingString:@"-wal"];

    _database = [[WCTDatabase alloc] initWithPath:_path];

    int tag;
    do {
        tag = [NSNumber randomInt32];
    } while (tag == 0);
    _database.tag = tag;

    _headerSize = 100;
    _walHeaderSize = 32;
    _walFrameHeaderSize = 24;

    _pageSize = 4096;
    _walFrameSize = _walFrameHeaderSize + _pageSize;
}

- (void)tearDown
{
    if (_database.isValidated) {
        [_database close];
        [_database invalidate];
    }
    _database = nil;
    [self cleanDirectory];
    [super tearDown];
}

- (NSNumber*)getFileSize:(NSString*)path
{
    NSError* error = nil;
    unsigned long long fileSize = [[self.fileManager attributesOfItemAtPath:path error:&error] fileSize];
    if (error) {
        return nil;
    }
    return @(fileSize);
}

- (int)getWalFrameCount
{
    NSInteger walSize = [self getFileSize:self.walPath].integerValue;
    if (walSize < self.walHeaderSize) {
        return 0;
    }
    return (int) ((walSize - self.walHeaderSize) / (self.walFrameHeaderSize + self.pageSize));
}

- (void)removeSQLRelatedConfigs
{
    NSArray<NSString*>* configNames = @[ WCTConfigNameBasic, WCTConfigNameBackup, WCTConfigNameCheckpoint, WCTConfigNameTokenize, WCTConfigNameCipher ];
    for (NSString* configName in configNames) {
        [self.database removeConfigForName:configName];
    }
}

- (BOOL)checkAllSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedInOperation:(BOOL (^)())block
{
    BOOL result = NO;
    do {
        __block BOOL trace = NO;
        if (![expectedSQLs isKindOfClass:NSArray.class]
            || expectedSQLs.count == 0
            || block == nil) {
            TESTCASE_FAILED
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (!trace) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                NSLog(@"Failed: %@", [TestCase hint:sql expecting:sqls.firstObject]);
                trace = NO;
                TESTCASE_FAILED
            }
        }];
        if (![self.database canOpen]) {
            TESTCASE_FAILED
            break;
        }

        trace = YES;
        @autoreleasepool {
            if (!block()) {
                TESTCASE_FAILED
                break;
            }
        }
        if (sqls.count != 0) {
            NSLog(@"Reminding: %@", sqls);
            TESTCASE_FAILED
            break;
        }
        trace = NO;
        result = YES;
    } while (false);
    [self.database traceSQL:nil];
    return result;
}

- (BOOL)checkBeginningSQLs:(NSArray<NSString*>*)expectedSQLs
     asExpectedInOperation:(BOOL (^)())block
{
    BOOL result = NO;
    do {
        __block BOOL trace = NO;
        if (![expectedSQLs isKindOfClass:NSArray.class]
            || expectedSQLs.count == 0
            || block == nil) {
            TESTCASE_FAILED
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (!trace || sqls.count == 0) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            }
        }];
        if (![self.database canOpen]) {
            TESTCASE_FAILED
            break;
        }

        trace = YES;
        @autoreleasepool {
            if (!block()) {
                TESTCASE_FAILED
                break;
            }
        }
        if (sqls.count != 0) {
            NSLog(@"Reminding: %@", sqls);
            TESTCASE_FAILED
            break;
        }
        trace = NO;
        result = YES;
    } while (false);
    [self.database traceSQL:nil];
    return result;
}

@end
