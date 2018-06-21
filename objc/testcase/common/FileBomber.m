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

#import "FileBomber.h"

@implementation FileBomber {
    NSFileHandle *_fileHandle;
}

- (instancetype)init
{
    return nil;
}

- (instancetype)initWithPath:(NSString *)path
{
    if (self = [super init]) {
        _attackType = FileBomberAttackTypeZero;
        _fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:path];
    }
    return self;
}

- (NSData *)armed:(int)size
{
    return [[NSMutableData alloc] initWithLength:size];
}

- (int)fileSize
{
    return (int) [_fileHandle seekToEndOfFile];
}

- (BOOL)attack:(NSRange)range
{
    unsigned long long fileSize = [_fileHandle seekToEndOfFile];
    if (range.location + range.length > fileSize) {
        range.length = fileSize - range.location;
    }
    if (range.length == 0) {
        return NO;
    }
    NSData *weapon = [self armed:(int) range.length];
    [_fileHandle writeData:weapon];
    return YES;
}

- (BOOL)randomAttack
{
    unsigned long long fileSize = self.fileSize;
    NSRange range;
    range.length = (arc4random() % fileSize) + 1;
    if (fileSize - range.length > 0) {
        range.location = arc4random() % (fileSize - range.length);
    } else {
        range.location = 0;
    }
    return [self attack:range];
}

@end

@implementation DatabaseBomber {
    FileBomber *_walFileBomber;
}

- (instancetype)initWithPath:(NSString *)path
{
    if (self = [super initWithPath:path]) {
        _walFileBomber = [[FileBomber alloc] initWithPath:[path stringByAppendingString:@"-wal"]];
        _pageSize = getpagesize();
    }
    return self;
}

- (int)pageCount
{
    unsigned long long fileSize = self.fileSize;
    return fileSize / _pageSize + (fileSize % _pageSize) > 0;
}

- (NSRange)pageRange:(int)pageno
{
    return NSMakeRange((pageno - 1) * _pageSize, _pageSize);
}

- (BOOL)attackRootPage
{
    return [self attackPage:1];
}

- (BOOL)attackPage:(int)pageno
{
    return [self attack:[self pageRange:pageno]];
}

- (BOOL)randomAttackPage
{
    int pageno = (arc4random() % self.pageCount) + 1;
    return [self attackPage:pageno];
}

- (int)walFrameSize
{
    return _pageSize + 24;
}

- (int)walFrameCount
{
    int framesSize = _walFileBomber.fileSize - 32;
    int frameSize = self.walFrameSize;
    return framesSize / frameSize + (framesSize % frameSize) > 0;
}

- (NSRange)walFrameRange:(int)frameno
{
    return NSMakeRange(32 + (frameno - 1) * self.walFrameSize, self.walFrameSize);
}

- (BOOL)attackWalHeader
{
    return [_walFileBomber attack:NSMakeRange(0, 32)];
}

- (BOOL)attackWalFrame:(int)frameno
{
    return [self attack:[self walFrameRange:frameno]];
}

- (BOOL)randomAttackWalFrame
{
    return [self attackWalFrame:(arc4random() % self.walFrameCount) + 1];
}

@end
