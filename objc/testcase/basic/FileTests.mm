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

#import <TestCase/TestCase.h>

@interface FileTests : DatabaseTestCase

@property (nonatomic, readonly) NSArray<NSString*>* dummyPaths;

@end

@implementation FileTests

- (void)setUp
{
    [super setUp];
    _dummyPaths = [self allDummyPaths:self.path];
}

- (NSArray<NSString*>*)allDummyPaths:(NSString*)database
{
    return @[
        database,
        [database stringByAppendingString:@"-wal"],
        [database stringByAppendingString:@"-first.material"],
        [database stringByAppendingString:@"-last.material"],
        [[[database stringByAppendingString:@".factory"] stringByAppendingPathComponent:@"restore"] stringByAppendingPathComponent:database.lastPathComponent],
        [database stringByAppendingString:@"-journal"],
        [database stringByAppendingString:@"-shm"],
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
    return [self allFilesExists:self.paths];
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
    return [self noneFilesExists:self.paths];
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

- (void)test_paths
{
    TestCaseAssertTrue([self.paths isEqualToArray:self.database.paths]);
}

- (void)test_remove_files
{
    TestCaseAssertTrue([self createDummyFiles]);
    TestCaseAssertTrue([self allFilesExists]);
    TestCaseAssertTrue([self.database canOpen]);
    TestCaseAssertTrue([self.database isOpened]);
    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertFalse([self.database isOpened]);
    TestCaseAssertTrue([self noneFilesExists]);
}

- (void)test_move_files
{
    NSString* filename = self.path.lastPathComponent;

    NSString* movedDirectory = [self.directory stringByAppendingPathComponent:@"moved"];
    NSString* movedDatabase = [movedDirectory stringByAppendingPathComponent:filename];
    NSArray<NSString*>* movedPaths = [self allDummyPaths:movedDatabase];

    TestCaseAssertTrue([self noneFilesExists:movedPaths]);

    TestCaseAssertTrue([self.database canOpen]);
    TestCaseAssertTrue([self.database isOpened]);

    TestCaseAssertTrue([self createDummyFiles]);
    TestCaseAssertTrue([self allFilesExists:self.dummyPaths]);
    TestCaseAssertTrue([self.database moveFilesToDirectory:movedDirectory]);

    TestCaseAssertFalse([self.database isOpened]);
    TestCaseAssertTrue([self noneFilesExists:self.dummyPaths]);
    TestCaseAssertTrue([self allFilesExists:movedPaths]);
}

- (void)test_files_size
{
    NSData* dummyData = [@"testData" dataUsingEncoding:NSUTF8StringEncoding];
    TestCaseAssertTrue([self createDummyFiles:dummyData]);
    NSInteger fileSize = [self.database getFilesSize];
    TestCaseAssertEqual(fileSize, dummyData.length * self.dummyPaths.count);
}

- (void)test_files_size_failed
{
    NSData* dummyData = [@"testData" dataUsingEncoding:NSUTF8StringEncoding];

    // bad guy
    TestCaseAssertTrue([self.fileManager removeItemAtPath:self.directory error:nil]);
    TestCaseAssertTrue([self.fileManager createFileAtPath:self.directory contents:dummyData attributes:nil]);

    auto fileSizeOptional = [self.database getFilesSize];
    TestCaseAssertTrue(fileSizeOptional.failed());
    TestCaseAssertEqual(fileSizeOptional.value(), 0);
}

- (void)test_feature_auto_create_immediate_directory
{
    NSString* directory = [[[[[[self.directory stringByAppendingPathComponent:@"immediate"] stringByAppendingPathComponent:@"directory"] stringByAppendingPathComponent:@"will"] stringByAppendingPathComponent:@"be"] stringByAppendingPathComponent:@"created"] stringByAppendingPathComponent:@"automatically"];
    NSString* path = [directory stringByAppendingPathComponent:self.path.lastPathComponent];

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    TestCaseAssertTrue([database canOpen]);

    BOOL isDirectory = NO;
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:directory isDirectory:&isDirectory]);
    TestCaseAssertTrue(isDirectory);
}

- (void)test_feature_path_normalized
{
    NSString* path = [self.directory stringByAppendingString:@"//directory//database"];
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    TestCaseAssertTrue([database canOpen]);

    NSString* normalizedPath = [[[NSString stringWithFormat:@"%@", self.directory] stringByAppendingPathComponent:@"directory"] stringByAppendingPathComponent:@"database"];
    TestCaseAssertTrue([database.path isEqualToString:normalizedPath]);
}

#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
- (void)test_feature_auto_set_file_protection
{
    TestCaseAssertTrue([self.database canOpen]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.path]);

    [self.database close];

    NSError* error;
    NSFileProtectionType fileProtectionTypeBefore = [self.fileManager getFileProtection:self.database.path error:&error];
    TestCaseAssertTrue(fileProtectionTypeBefore == nil
                       || [fileProtectionTypeBefore isEqualToString:NSFileProtectionNone]
                       || [fileProtectionTypeBefore isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication]);
    TestCaseAssertTrue(error == nil);

    TestCaseAssertTrue([self.fileManager setFileProtectionOfPath:self.database.path to:NSFileProtectionComplete error:&error]);
    TestCaseAssertTrue(error == nil);

    // trigger
    TestCaseAssertTrue([self.database canOpen]);

    NSFileProtectionType fileProtectionTypeAfter = [self.fileManager getFileProtection:self.database.path error:&error];
    TestCaseAssertTrue([fileProtectionTypeAfter isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication]);
    TestCaseAssertTrue(error == nil);
}
#endif

@end
