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

@interface DatabaseTests : TestCase

@end

@implementation DatabaseTests {
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

- (void)test_open
{
    XCTAssertFalse([_database isOpened]);
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue([_database isOpened]);
}

- (void)test_tag
{
    WCTDatabase *database2 = [[WCTDatabase alloc] initWithPath:_database.path];
    _database.tag = rand();
    XCTAssertEqual(_database.tag, database2.tag);
}

- (void)test_existing_tag
{
    _database.tag = rand();
    WCTDatabase *database2 = [[WCTDatabase alloc] initWithExistingTag:_database.tag];
    XCTAssertNotNil(database2);
    XCTAssertTrue([_database.path isEqualToString:database2.path]);
}

- (void)test_path
{
    NSString *document = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, NO)[0];
    NSString *tildeExpandedDocument = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)[0];

    NSString *origin = [tildeExpandedDocument stringByAppendingPathComponent:self.testname];
    _database = [[WCTDatabase alloc] initWithPath:origin];
    WCTTag tag = (WCTTag) self.testname.hash;
    _database.tag = tag;

    NSString *symboliclink = [origin stringByAppendingString:@"_symboliclink"];
    XCTAssertTrue([self.fileManager createSymbolicLinkAtPath:symboliclink withDestinationPath:origin error:nil]);
    WCTDatabase *databaseFromSymboliclink = [[WCTDatabase alloc] initWithPath:symboliclink];
    XCTAssertTrue([databaseFromSymboliclink.path isEqualToString:symboliclink]);
    XCTAssertEqual(databaseFromSymboliclink.tag, tag);
    XCTAssertFalse([databaseFromSymboliclink.path hasPrefix:@"~/"]);

    NSString *hardlink = [origin stringByAppendingString:@"_hardlink"];
    XCTAssertTrue([self.fileManager linkItemAtPath:hardlink toPath:origin error:nil]);
    WCTDatabase *databaseFromHardlink = [[WCTDatabase alloc] initWithPath:hardlink];
    XCTAssertTrue([databaseFromHardlink.path isEqualToString:hardlink]);
    XCTAssertEqual(databaseFromHardlink.tag, tag);
    XCTAssertFalse([databaseFromHardlink.path hasPrefix:@"~/"]);

    NSString *tilde = [[document stringByAppendingPathComponent:self.testname] stringByAppendingString:@"_tilde"];
    WCTDatabase *databaseFromTilde = [[WCTDatabase alloc] initWithPath:tilde];
    XCTAssertTrue([databaseFromTilde.path isEqualToString:tilde]);
    XCTAssertEqual(databaseFromTilde.tag, tag);
    XCTAssertTrue([databaseFromTilde.path hasPrefix:@"~/"]);
}

- (void)test_blockade
{
    [_database blockade];
    NSDate *main;
    __block NSDate *subthread;
    dispatch_group_t group = dispatch_group_create();
    dispatch_group_async(group, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        XCTAssertTrue([_database canOpen]);
        @synchronized(self) {
            subthread = [NSDate date];
        }
    });
    [NSThread sleepForTimeInterval:3];
    main = [NSDate date];
    [_database unblockade];
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
    XCTAssertTrue([main compare:subthread] == NSOrderedAscending);
}

- (void)test_inside_blockade
{
    dispatch_group_t group = dispatch_group_create();
    __block NSDate *main;
    __block NSDate *subthread;
    BOOL result = [_database blockadeUntilDone:^(WCTHandle *handle) {
        XCTAssertTrue([_database isBlockaded]);
        dispatch_group_async(group, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            XCTAssertTrue([_database canOpen]);
            @synchronized(self) {
                subthread = [NSDate date];
            }
        });
        [NSThread sleepForTimeInterval:3];
        main = [NSDate date];
    }];
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
    XCTAssertTrue([main compare:subthread] == NSOrderedAscending);
    XCTAssertTrue(result);
}

- (void)test_checkpoint
{
    NSString *tableName = NSStringFromSelector(_cmd);
    XCTAssertTrue([_database createTableAndIndexes:tableName withClass:TestCaseObject.class]);
    int count = 100;
    int pageSize = 4096;
    for (int i = 0; i < count; ++i) {
        TestCaseObject *object = [TestCaseObject objectWithId:i];
        XCTAssertTrue([_database insertObject:object intoTable:tableName]);
    }
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *walPath = [_database.path stringByAppendingString:@"-wal"];
    unsigned long long fileSize = [[fileManager attributesOfItemAtPath:walPath error:nil] fileSize];
    XCTAssertGreaterThan(fileSize, count * pageSize);
    [NSThread sleepForTimeInterval:5];
    TestCaseObject *object = [TestCaseObject objectWithId:count];
    XCTAssertTrue([_database insertObject:object intoTable:tableName]);
    unsigned long long newFileSize = [[fileManager attributesOfItemAtPath:walPath error:nil] fileSize];
    XCTAssertEqual(fileSize, newFileSize);
}

- (void)test_create_intermediate_directories
{
    NSString *longPath = self.recommendedDirectory;
    for (int i = 0; i < 10; ++i) {
        NSString *intermediate = [NSString stringWithFormat:@"%d", i];
        longPath = [longPath stringByAppendingPathComponent:intermediate];
    }
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:longPath];
    XCTAssertTrue([database canOpen]);
    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:longPath]);
}

- (void)test_close
{
    NSString *tableName = NSStringFromSelector(_cmd);
    XCTAssertTrue([_database createTableAndIndexes:tableName withClass:TestCaseObject.class]);
    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:_database.path]);
    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:[_database.path stringByAppendingString:@"-wal"]]);

    [_database close];

    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:_database.path]);
    XCTAssertFalse([[NSFileManager defaultManager] fileExistsAtPath:[_database.path stringByAppendingPathExtension:@"-wal"]]);
}

- (void)test_dealloc
{
    NSString *cmdName = NSStringFromSelector(_cmd);
    NSString *path = [NSString stringWithFormat:@"%@-%@", self.recommendedPath, cmdName];
    @autoreleasepool {
        WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
        XCTAssertTrue([database createTableAndIndexes:cmdName withClass:TestCaseObject.class]);
        XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:database.path]);
        XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:[database.path stringByAppendingString:@"-wal"]]);
    }

    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    XCTAssertFalse([database isOpened]);
    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:database.path]);
    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:[database.path stringByAppendingString:@"-wal"]]);
}

@end
