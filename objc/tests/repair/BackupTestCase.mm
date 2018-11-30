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

@implementation BackupTestCase

- (void)setUp
{
    [super setUp];
    _firstMaterial = [NSString stringWithFormat:@"%@-first.material", self.path];
    _lastMaterial = [NSString stringWithFormat:@"%@-last.material", self.path];
    _factory = [NSString stringWithFormat:@"%@.factory", self.path];

    _backupFramesIntervalForNonCritical = 100;
    _backupFramesIntervalForCritical = 300;
    _backupDelayForCritical = 0;
    _backupDelayForNonCritical = 1.0;

    _delayForTolerance = 1.0;
    _framesForTolerance = 10;
}

- (BOOL)tryToMakeHeaderCorrupted
{
    if (![self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]) {
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
