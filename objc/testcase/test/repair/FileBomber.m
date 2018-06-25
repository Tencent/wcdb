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
#import "Convenience.h"

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
        _fileSize = (int) [_fileHandle seekToEndOfFile];
    }
    return self;
}

- (NSData *)armed:(int)size
{
    NSData *data;
    if (self.attackType == FileBomberAttackTypeRandom) {
        data = [NSData randomDataWithLength:size];
    } else {
        data = [[NSMutableData alloc] initWithLength:size];
    }
    return data;
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
    [_fileHandle seekToFileOffset:range.location];
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
    NSMutableSet *_attacked;
}

- (instancetype)initWithPath:(NSString *)path
{
    if (self = [super initWithPath:path]) {
        _walFileBomber = [[FileBomber alloc] initWithPath:[path stringByAppendingString:@"-wal"]];
        _pageSize = getpagesize();
        _attacked = [[NSMutableSet alloc] init];
        _pageCount = (int) (self.fileSize / _pageSize) + ((self.fileSize % _pageSize) > 0);
    }
    return self;
}

- (BOOL)isPageAttacked:(int)pageno
{
    return [_attacked containsObject:@(pageno)];
}

- (NSRange)pageRange:(int)pageno
{
    return NSMakeRange((pageno - 1) * self.pageSize, self.pageSize);
}

- (BOOL)attackRootPage
{
    return [self attackPage:1];
}

- (BOOL)attackPage:(int)pageno
{
    if ([self isPageAttacked:pageno]) {
        return NO;
    }
    BOOL result = [self attack:[self pageRange:pageno]];
    if (result) {
        [_attacked addObject:@(pageno)];
    }
    return result;
}

- (BOOL)randomAttackPage
{
    int pageno;
    do {
        pageno = (arc4random() % self.pageCount) + 1;
    } while ([self isPageAttacked:pageno]);
    return [self attackPage:pageno];
}

- (int)walFrameSize
{
    return self.pageSize + 24;
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
