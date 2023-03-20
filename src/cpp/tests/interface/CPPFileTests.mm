//
// Created by qiuwenchen on 2022/8/10.
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

#import "CPPTestCase.h"

@interface CPPFileTests : CPPDatabaseTestCase

@property (nonatomic, readonly) NSArray<NSString*>* dummyPaths;

@end

@implementation CPPFileTests

- (void)setUp
{
    [super setUp];
    _dummyPaths = [self allDummyPaths:self.path];
}

- (NSArray<NSString*>*)allDummyPaths:(NSString*)path
{
    return @[
        path,
        [path stringByAppendingString:@"-wal"],
        [path stringByAppendingString:@"-first.material"],
        [path stringByAppendingString:@"-last.material"],
        [[path stringByAppendingString:@".factory"] stringByAppendingPathComponent:path.lastPathComponent],
        [path stringByAppendingString:@"-journal"],
        [path stringByAppendingString:@"-shm"],
    ];
}

- (BOOL)createDummyFiles
{
    return [self createDummyFiles:[NSData data]];
}

- (BOOL)createDummyFiles:(NSData*)data
{
    if (data == nil) {
        TestCaseFailure();
        return NO;
    }
    for (NSString* path in self.dummyPaths) {
        if (![self.fileManager createDirectoryAtPath:path.stringByDeletingLastPathComponent withIntermediateDirectories:YES attributes:nil error:nil]) {
            TestCaseFailure();
            return NO;
        }
        if ([self.fileManager fileExistsAtPath:path]) {
            continue;
        }
        if (![self.fileManager createFileAtPath:path contents:data attributes:nil]) {
            TestCaseFailure();
            return NO;
        }
    }
    return YES;
}

- (BOOL)allFilesExists
{
    return [self allFilesExists:[self getAllFilePaths]];
}

- (NSArray*)getAllFilePaths
{
    NSMutableArray* paths = [[NSMutableArray alloc] init];
    for (const WCDB::UnsafeStringView& path : self.database->getPaths()) {
        [paths addObject:[NSString stringWithUTF8String:path.data()]];
    }
    return paths;
}

- (BOOL)allFilesExists:(NSArray<NSString*>*)paths
{
    for (NSString* path in paths) {
        if (![self.fileManager fileExistsAtPath:path]) {
            TestCaseFailure();
            return NO;
        }
    }
    return YES;
}

- (BOOL)noneFilesExists
{
    return [self noneFilesExists:[self getAllFilePaths]];
}

- (BOOL)noneFilesExists:(NSArray<NSString*>*)paths
{
    for (NSString* path in paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            TestCaseFailure();
            return NO;
        }
    }
    return YES;
}

- (void)test_remove_files
{
    TestCaseAssertTrue([self createDummyFiles]);
    TestCaseAssertTrue([self allFilesExists]);
    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertTrue(self.database->isOpened());
    TestCaseAssertTrue(self.database->removeFiles());
    TestCaseAssertFalse(self.database->isOpened());
    TestCaseAssertTrue([self noneFilesExists]);
}

- (void)test_move_files
{
    NSString* filename = self.path.lastPathComponent;

    NSString* movedDirectory = [self.directory stringByAppendingPathComponent:@"moved"];
    NSString* movedDatabase = [movedDirectory stringByAppendingPathComponent:filename];
    NSArray<NSString*>* movedPaths = [self allDummyPaths:movedDatabase];

    TestCaseAssertTrue([self noneFilesExists:movedPaths]);

    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertTrue(self.database->isOpened());

    TestCaseAssertTrue([self createDummyFiles]);
    TestCaseAssertTrue([self allFilesExists:self.dummyPaths]);
    TestCaseAssertTrue(self.database->moveFiles(movedDirectory.UTF8String));

    TestCaseAssertFalse(self.database->isOpened());
    TestCaseAssertTrue([self noneFilesExists:self.dummyPaths]);
    TestCaseAssertTrue([self allFilesExists:movedPaths]);
}

- (void)test_files_size
{
    NSData* dummyData = [@"testData" dataUsingEncoding:NSUTF8StringEncoding];
    TestCaseAssertTrue([self createDummyFiles:dummyData]);
    NSInteger fileSize = self.database->getFilesSize().value();
    TestCaseAssertEqual(fileSize, dummyData.length * self.dummyPaths.count);
}

- (void)test_files_size_failed
{
    NSData* dummyData = [@"testData" dataUsingEncoding:NSUTF8StringEncoding];

    // bad guy
    TestCaseAssertTrue([self.fileManager removeItemAtPath:self.directory error:nil]);
    TestCaseAssertTrue([self.fileManager createFileAtPath:self.directory contents:dummyData attributes:nil]);

    auto fileSizeOptional = self.database->getFilesSize();
    TestCaseAssertTrue(!fileSizeOptional.succeed());
}

- (void)test_feature_auto_create_immediate_directory
{
    NSString* directory = [[[[[[self.directory stringByAppendingPathComponent:@"immediate"] stringByAppendingPathComponent:@"directory"] stringByAppendingPathComponent:@"will"] stringByAppendingPathComponent:@"be"] stringByAppendingPathComponent:@"created"] stringByAppendingPathComponent:@"automatically"];
    NSString* path = [directory stringByAppendingPathComponent:self.path.lastPathComponent];

    WCDB::Database database(path.UTF8String);
    TestCaseAssertTrue(database.canOpen());

    BOOL isDirectory = NO;
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:directory isDirectory:&isDirectory]);
    TestCaseAssertTrue(isDirectory);
}

- (void)test_feature_path_normalized
{
    NSString* path = [self.directory stringByAppendingString:@"//directory//database"];
    WCDB::Database database(path.UTF8String);
    TestCaseAssertTrue(database.canOpen());

    NSString* normalizedPath = [[[NSString stringWithFormat:@"%@", self.directory] stringByAppendingPathComponent:@"directory"] stringByAppendingPathComponent:@"database"];
    TestCaseAssertTrue([[NSString stringWithUTF8String:database.getPath().data()] isEqualToString:normalizedPath]);
}

@end
