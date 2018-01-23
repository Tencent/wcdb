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

#import "WTCBaseTestCase.h"
#import "Convenience.h"

@interface WTCFileTests : WTCBaseTestCase
@property(nonatomic, readonly) WCTDatabase *database;
@end

@implementation WTCFileTests 

- (void)setUp {
    [super setUp];
    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
}

- (void)tearDown
{
    [_database close:^{
        XCTAssertTrue([_database removeFilesWithError:nil]);
    }];
    _database = nil;
    [super tearDown];
}

- (void)testPaths {
    //Give
    NSString *path = self.recommendedPath;
    NSArray<NSString*>* expectedPaths = @[path, [path stringByAppendingString:@"-wal"], [path stringByAppendingString:@"-shm"], [path stringByAppendingString:@"-journal"], [path stringByAppendingString:@"-backup"]];
    //Then
    NSArray<NSString*>* sortedPaths = [self.database.paths sortedArrayUsingComparator:NSString.Comparator];
    NSArray<NSString*>* sortedExpectedPaths = [expectedPaths sortedArrayUsingComparator:NSString.Comparator];    
    XCTAssertTrue([sortedPaths isEqual:sortedExpectedPaths]);
}

- (void)testRemoveFiles {
    //Give
    for (NSString* path in self.database.paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:nil attributes:nil]);
    }
    //When
    [self.database close:^{
        XCTAssertTrue([self.database removeFilesWithError:nil]);
    }];
    //Then
    for (NSString *path in self.database.paths) {
        XCTAssertFalse([self.fileManager fileExistsAtPath:path]);
    }
}

- (void)testUnsafeRemoveFiles {
    //Give
    for (NSString* path in self.database.paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:nil attributes:nil]);
    }
    //When
    XCTAssertTrue([self.database removeFilesWithError:nil]);
    //Then
    for (NSString *path in self.database.paths) {
        XCTAssertFalse([self.fileManager fileExistsAtPath:path]);
    }
}

- (void)testMoveFiles
{
    //Give
    NSString* extraFile = [self.recommendedDirectory stringByAppendingPathComponent:@"extraFile"];
    NSArray<NSString*>* paths = [self.database.paths arrayByAddingObject:extraFile];
    for (NSString *path in paths) {
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:nil attributes:nil]);        
    }
    
    NSString *newDirectory = [self.recommendedDirectory stringByAppendingPathComponent:@"newDirectory"];
    
    NSMutableArray<NSString*>* newPaths = [[NSMutableArray<NSString*> alloc] init];
    for (NSString* path in paths) {
        NSString* fileName = path.lastPathComponent;
        [newPaths addObject:[newDirectory stringByAppendingPathComponent:fileName]];
    }
    
    for (NSString* path in newPaths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
    }
    
    //When
    XCTAssertTrue([self.database moveFilesToDirectory:newDirectory withExtraFiles:@[extraFile] andError:nil]);
    //Then
    for (NSString *path in newPaths) {
        XCTAssertTrue([self.fileManager fileExistsAtPath:path]);
    }
    
    //Clear
    XCTAssertTrue([self.fileManager removeItemAtPath:newDirectory error:nil]);
}

- (void)testGetFilesSize
{
    //Give
    NSData *data = [@"testGetFilesSize" dataUsingEncoding:NSASCIIStringEncoding];
    NSUInteger expectedFilesSize = data.length * self.database.paths.count;
    for (NSString* path in self.database.paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:data attributes:nil]);
    }
    //Then
    [self.database close:^{
        NSUInteger filesSize = [self.database getFilesSizeWithError:nil];
        XCTAssertEqual(filesSize, expectedFilesSize);
    }];    
}

- (void)testUnsafeGetFilesSize
{
    //Give
    NSData *data = [@"testGetFilesSize" dataUsingEncoding:NSASCIIStringEncoding];
    NSUInteger expectedFilesSize = data.length * self.database.paths.count;
    for (NSString* path in self.database.paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:path error:nil]);
        }
        XCTAssertTrue([self.fileManager createFileAtPath:path contents:data attributes:nil]);
    }
    //Then
    NSUInteger filesSize = [self.database getFilesSizeWithError:nil];
    XCTAssertEqual(filesSize, expectedFilesSize);
}


@end
