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

#import "TestCaseCommon.h"

@interface DatabaseTests_File : TestCase
@end

@implementation DatabaseTests_File {
    WCTDatabase *_database;
}

- (void)setUp
{
    [super setUp];
    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
}

- (void)tearDown
{
    XCTAssertTrue([_database removeFiles]);
    _database = nil;
    [super tearDown];
}

- (void)test_auto_create_intermediate
{
    //TODO
}

- (void)test_path_with_normalized
{
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue([_database isOpened]);
    NSString *path = [_database.path stringByReplacingOccurrencesOfString:@"/" withString:@"//"];
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    XCTAssertTrue([database isOpened]);
    XCTAssertTrue([_database.path isEqualToString:database.path]);
}

//TODO test materials
- (void)test_paths
{
    //Give
    NSString *path = self.recommendedPath;
    NSArray<NSString *> *expectedPaths = @[
        path,
        [path stringByAppendingString:@"-wal"],
        [path stringByAppendingString:@"-first.material"],
        [path stringByAppendingString:@"-last.material"],
        [path stringByAppendingString:@".factory"],
        [path stringByAppendingString:@"-journal"],
        [path stringByAppendingString:@"-shm"],
    ];
    //Then
    XCTAssertTrue(([_database.paths isEqualToObjects:expectedPaths
                                      withComparator:^BOOL(NSString *lhs, NSString *rhs) {
                                          return [lhs isEqualToString:rhs];
                                      }]));
}

- (void)test_remove_files
{
    //Give
    for (NSString *path in _database.paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:nil attributes:nil]);
    }
    //When
    [_database close:^{
        //also tests recursive close
        [_database close:^{
            XCTAssertTrue([_database removeFiles]);
        }];
    }];
    //Then
    for (NSString *path in _database.paths) {
        XCTAssertFalse([self.fileManager fileExistsAtPath:path]);
    }
}

- (void)test_move_files
{
    //Give
    NSString *extraFile = [self.recommendedDirectory stringByAppendingPathComponent:@"extraFile"];
    NSArray<NSString *> *paths = [_database.paths arrayByAddingObject:extraFile];
    for (NSString *path in paths) {
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:nil attributes:nil]);
    }

    NSString *newDirectory = [self.recommendedDirectory stringByAppendingPathComponent:@"newDirectory"];

    NSMutableArray<NSString *> *newPaths = [[NSMutableArray<NSString *> alloc] init];
    for (NSString *path in paths) {
        NSString *fileName = path.lastPathComponent;
        [newPaths addObject:[newDirectory stringByAppendingPathComponent:fileName]];
    }

    for (NSString *path in newPaths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
    }

    //When
    [_database close:^{
        XCTAssertTrue([_database moveFilesToDirectory:newDirectory withExtraFiles:@[ extraFile ]]);
    }];
    //Then
    for (NSString *path in newPaths) {
        XCTAssertTrue([self.fileManager fileExistsAtPath:path]);
    }

    //Clear
    XCTAssertTrue([self.fileManager removeItemAtPath:newDirectory error:nil]);
}

- (void)test_get_files_size
{
    //Give
    NSData *data = [@"testGetFilesSize" dataUsingEncoding:NSASCIIStringEncoding];
    NSUInteger expectedFilesSize = data.length * _database.paths.count;
    for (NSString *path in _database.paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:data attributes:nil]);
    }
    //Then
    [_database close:^{
        NSUInteger filesSize = [_database getFilesSize];
        XCTAssertEqual(filesSize, expectedFilesSize);
    }];
}

#if TARGET_OS_IPHONE
- (void)test_file_protection
{
    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    XCTAssertTrue([_database createTableAndIndexes:self.className
                                         withClass:TestCaseObject.class]);
    XCTAssertTrue([_database backup]);
    [NSThread sleepForTimeInterval:1.0];
    XCTAssertTrue([_database backup]);
    NSArray<NSString *> *paths = @[
        _database.path,
        [_database.path stringByAppendingString:@"-shm"],
        [_database.path stringByAppendingString:@"-wal"],
        [_database.path stringByAppendingString:@"-first.material"],
        [_database.path stringByAppendingString:@"-last.material"],
    ];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    for (NSString *path in paths) {
        NSDictionary *attributes = [fileManager attributesOfItemAtPath:path error:nil];
        NSString *fileProtection = [attributes objectForKey:NSFileProtectionKey];
        XCTAssertTrue([fileProtection isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication] || [fileProtection isEqualToString:NSFileProtectionNone]);
    }
}
#endif

@end
