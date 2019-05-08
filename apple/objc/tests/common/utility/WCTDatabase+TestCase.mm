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
#import "WCTDatabase+TestCase.h"

@interface WCTDatabase (TestCase1)

+ (void)globalTraceError:(WCTErrorTraceBlock)block
          withIdentifier:(const WCDB::String &)identifier;

@end

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

- (void)removeCheckpointConfig
{
    [self removeConfigForName:@(WCDB::CheckpointConfigName)];
}

- (WCTOptionalSize)getNumberOfPages
{
    WCTOptionalSize size = nullptr;
    NSError *error;
    NSNumber *nsSize = [[NSFileManager defaultManager] attributesOfItemAtPath:self.path error:&error][NSFileSize];
    if (nsSize != nil && error == nil) {
        int numberOfPages = (int) (nsSize.integerValue / self.pageSize);
        size = numberOfPages > 0 ? numberOfPages : 0;
    }
    return size;
}

- (WCTOptionalSize)getNumberOfWalFrames
{
    WCTOptionalSize size = nullptr;
    NSError *error;
    NSNumber *nsSize = [[NSFileManager defaultManager] attributesOfItemAtPath:self.walPath error:&error][NSFileSize];
    if (nsSize != nil && error == nil) {
        int numberOfFrames = (int) ((nsSize.integerValue - self.walHeaderSize) / (self.walFrameHeaderSize + self.pageSize));
        size = numberOfFrames > 0 ? numberOfFrames : 0;
    }
    return size;
}

+ (void)additionalGlobalTraceError:(WCTErrorTraceBlock)block
{
    [self globalTraceError:block withIdentifier:"com.Tencent.WCDB.Notifier.AdditionalLog"];
}

@end
