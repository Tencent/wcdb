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

#import "TestCase.h"

@implementation TestCase

+ (void)initialize
{
    srand((unsigned int) time(NULL));
}

+ (NSString *)className
{
    return NSStringFromClass(self.class);
}

- (NSString *)className
{
    return self.class.className;
}

+ (NSString *)baseDirectory
{
    NSString *name = [NSString stringWithFormat:@"WCDB_%@", TestCase.className];
    return [NSTemporaryDirectory() stringByAppendingPathComponent:name];
}

- (NSString *)recommendedDirectory
{
    return [self.class.baseDirectory stringByAppendingPathComponent:self.className];
}

- (NSString *)recommendedPath
{
    NSString *hash = [NSString stringWithFormat:@"%lu", (unsigned long) self.className.hash];
    return [self.recommendedDirectory stringByAppendingPathComponent:hash];
}

- (WCTTag)recommendedTag
{
    return (int) self.recommendedPath.hash;
}

+ (NSString *)cachedDirectory
{
    return [self.baseDirectory stringByAppendingPathComponent:@"cached"];
}

- (NSFileManager *)fileManager
{
    return [NSFileManager defaultManager];
}

- (void)setUp
{
    [super setUp];

    self.continueAfterFailure = YES;

    if ([self.fileManager fileExistsAtPath:self.recommendedDirectory]) {
        XCTAssertTrue([self.fileManager removeItemAtPath:self.recommendedDirectory error:nil]);
    }
    XCTAssertTrue([self.fileManager createDirectoryAtPath:self.recommendedDirectory
                              withIntermediateDirectories:YES
                                               attributes:nil
                                                    error:nil]);

    NSLog(@"Tests at %@", self.recommendedDirectory);
}

- (void)tearDown
{
    if ([self.fileManager fileExistsAtPath:self.recommendedDirectory]) {
        XCTAssertTrue([self.fileManager removeItemAtPath:self.recommendedDirectory error:nil]);
    }
}

@end
