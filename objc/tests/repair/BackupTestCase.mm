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

#import "BackupTestCase.h"
#import <WCDB/CoreConst.h>

@implementation BackupTestCase

- (void)setUp
{
    [super setUp];
    _firstMaterial = [self.path stringByAppendingString:@"-first.material"];
    _lastMaterial = [self.path stringByAppendingString:@"-last.material"];
    _factory = [self.path stringByAppendingString:@".factory"];

    _backupFramesIntervalForNonCritical = WCDB::BackupConfigFramesIntervalForNonCritical;
    _backupFramesIntervalForCritical = WCDB::BackupConfigFramesIntervalForCritical;
    _backupDelayForCritical = WCDB::BackupConfigDelayForCritical;
    _backupDelayForNonCritical = WCDB::BackupConfigDelayForNonCritical;

    _delayForTolerance = 1.0;
    _framesForTolerance = 10;
}

- (BOOL)tryToMakeHeaderCorrupted
{
    if (![self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")]) {
        TESTCASE_FAILED
        return NO;
    }
    __block BOOL result = NO;
    [self.database close:^{
        NSFileHandle* fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:self.path];
        if (!fileHandle) {
            TESTCASE_FAILED
            return;
        }
        [fileHandle writeData:[NSData randomDataWithLength:self.headerSize]];
        [fileHandle closeFile];
        result = YES;
    }];
    return result;
}

@end
