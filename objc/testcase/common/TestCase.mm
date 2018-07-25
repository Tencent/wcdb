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

+ (NSString *)directory
{
    NSString *name = [NSString stringWithFormat:@"WCDB_%@", TestCase.className];
    return [NSTemporaryDirectory() stringByAppendingPathComponent:name];
}

+ (NSString *)cacheDirectory
{
    return [self.directory stringByAppendingPathComponent:@"Cache"];
}

+ (NSString *)className
{
    return NSStringFromClass(self.class);
}

- (NSString *)className
{
    return self.class.className;
}

- (NSString *)testname
{
    NSString *name = self.name;
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"\\[\\w+ (\\w+).*\\]" options:NSRegularExpressionCaseInsensitive error:nil];
    NSTextCheckingResult *match = [regex firstMatchInString:name options:0 range:NSMakeRange(0, [name length])];
    return [name substringWithRange:[match rangeAtIndex:1]];
}

- (WCTTag)recommendedTag
{
    return (int) self.recommendedPath.hash;
}

- (NSString *)recommendedDirectory
{
    return [self.class.directory stringByAppendingPathComponent:self.className];
}

- (NSString *)recommendedPath
{
    return [self.recommendedDirectory stringByAppendingPathComponent:self.testname];
}

- (NSString *)cacheDirectory
{
    return [self.class.cacheDirectory stringByAppendingPathComponent:self.className];
}

- (NSString *)cachePath
{
    return [self.cacheDirectory stringByAppendingPathComponent:self.testname];
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
}

- (void)tearDown
{
    if ([self.fileManager fileExistsAtPath:self.recommendedDirectory]) {
        XCTAssertTrue([self.fileManager removeItemAtPath:self.recommendedDirectory error:nil]);
    }
}

@end
