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

#import "CoreConst.h"
#import "TestCaseLog.h"
#import "WCTDatabase+TestCase.h"
#if TEST_WCDB_OBJC
#import <WCDBOBjc/WCTDatabase+Test.h>
#elif TEST_WCDB_CPP
#import <WCDBCpp/WCTDatabase+Test.h>
#else
#import <WCDB/WCTDatabase+Test.h>
#endif

@implementation WCTDatabase (TestCase)

- (NSString *)walPath
{
    return [self.path stringByAppendingString:@"-wal"];
}

- (NSString *)firstMaterialPath
{
    return [self.path stringByAppendingString:@"-first.material"];
}

- (NSString *)lastMaterialPath
{
    return [self.path stringByAppendingString:@"-last.material"];
}

- (NSString *)factoryPath
{
    return [self.path stringByAppendingString:@".factory"];
}

- (NSString *)journalPath
{
    return [self.path stringByAppendingString:@"-journal"];
}

- (NSString *)shmPath
{
    return [self.path stringByAppendingString:@"-shm"];
}

- (NSString *)factoryRestorePath
{
    return [self.factoryPath stringByAppendingPathComponent:@"restore"];
}

- (int)headerSize
{
    return 100;
}

- (int)pageSize
{
    return 4096;
}

- (int)walHeaderSize
{
    return 32;
}

- (int)walFrameHeaderSize
{
    return 24;
}

- (int)walFrameSize
{
    return self.walFrameHeaderSize + self.pageSize;
}

- (WCTOptionalSize)getNumberOfPages
{
    WCTOptionalSize result;
    if ([[NSFileManager defaultManager] fileExistsAtPath:self.path]) {
        NSError *error;
        size_t size = (size_t) ((NSNumber *) [[NSFileManager defaultManager] attributesOfItemAtPath:self.path error:&error][NSFileSize]).unsignedLongLongValue;
        if (error == nil) {
            int numberOfPages = (int) (size / self.pageSize);
            result = numberOfPages > 0 ? numberOfPages : 0;
        } else {
            TestCaseLog(@"%@", error);
        }
    } else {
        result = 0;
    }
    return result;
}

- (WCTOptionalSize)getNumberOfWalFrames
{
    WCTOptionalSize result;
    NSError *error;
    if ([[NSFileManager defaultManager] fileExistsAtPath:self.walPath]) {
        size_t size = (size_t) ((NSNumber *) [[NSFileManager defaultManager] attributesOfItemAtPath:self.walPath error:&error][NSFileSize]).unsignedLongLongValue;
        if (error == nil) {
            if (size == 0) {
                result = 0;
            } else if (size > self.walHeaderSize) {
                if ((size - self.walHeaderSize) % self.walFrameSize == 0) {
                    result = (size - self.walHeaderSize) / self.walFrameSize;
                }
            }
        } else {
            TestCaseLog(@"%@", error);
        }
    } else {
        result = 0;
    }
    return result;
}

- (BOOL)corruptHeaderWithinCloseAfterTruncatedCheckpoint
{
    __block BOOL result = NO;
    [self close:^{
        result = [self truncateCheckpoint];

        NSFileHandle *fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:self.path];
        NSMutableData *emptyData = [NSMutableData data];
        [emptyData increaseLengthBy:self.headerSize];
        [fileHandle writeData:emptyData];
        [fileHandle closeFile];
    }];
    return result;
}

- (void)corruptWalFrame:(int)i
{
    NSFileHandle *fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:self.walPath];
    [fileHandle seekToFileOffset:self.walHeaderSize + (i - 1) * self.walFrameSize];
    NSMutableData *emptyData = [NSMutableData data];
    [emptyData increaseLengthBy:self.walFrameSize];
    [fileHandle writeData:emptyData];
    [fileHandle closeFile];
}

- (void)corruptPage:(int)i
{
    [self corruptPage:i
             withData:^NSData *(int size) {
                 return [NSMutableData dataWithLength:size];
             }];
}

- (void)corruptPage:(int)i withData:(WCTCorruptDataBlock)block
{
    NSFileHandle *fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:self.path];
    [fileHandle seekToFileOffset:(i - 1) * self.pageSize];
    [fileHandle writeData:block(self.pageSize)];
    [fileHandle closeFile];
}

- (BOOL)corruptCompletely
{
    auto numberOfPages = [self getNumberOfPages];
    if (numberOfPages.failed()) {
        return NO;
    }
    for (int i = 0; i < numberOfPages.value(); ++i) {
        [self corruptPage:i + 1];
    }
    auto numberOfWalFrames = [self getNumberOfWalFrames];
    if (numberOfWalFrames.failed()) {
        return NO;
    }
    for (int i = 0; i < numberOfWalFrames.value(); ++i) {
        [self corruptWalFrame:i + 1];
    }
    return YES;
}

- (WCTOptionalBool)isAlreadyCheckpointed
{
    WCTOptionalSize frames = [self getNumberOfWalFrames];
    if (frames.failed()) {
        return WCDB::NullOpt;
    }
    if (frames.value() == 0) {
        return YES;
    }

    BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:self.shmPath];
    if (!exists) {
        return WCDB::NullOpt;
    }

    NSFileHandle *fileHandle = [NSFileHandle fileHandleForReadingAtPath:self.shmPath];

    [fileHandle seekToFileOffset:16];
    NSData *data = [fileHandle readDataOfLength:4];
    if (data == nil) {
        return WCDB::NullOpt;
    }
    uint32_t maxFrame = *(uint32_t *) data.bytes;

    [fileHandle seekToFileOffset:96];
    data = [fileHandle readDataOfLength:4];
    if (data == nil) {
        return WCDB::NullOpt;
    }
    uint32_t backfill = *(uint32_t *) data.bytes;

    [fileHandle closeFile];

    return backfill >= maxFrame;
}

- (WCTOptionalBool)isAlreadyTruncateCheckpointed
{
    WCTOptionalSize frames = [self getNumberOfWalFrames];
    if (frames.failed()) {
        return WCDB::NullOpt;
    }
    return frames.value() == 0;
}

@end
