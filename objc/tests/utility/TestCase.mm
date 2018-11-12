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
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
#ifdef DEBUG
        WCTDatabase.debuggable = YES;
#else
        WCTDatabase.debuggable = NO;
#endif
    });
}

- (NSString *)testName
{
    NSString *name = self.name;
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"\\[\\w+ (\\w+).*\\]" options:NSRegularExpressionCaseInsensitive error:nil];
    NSTextCheckingResult *match = [regex firstMatchInString:name options:0 range:NSMakeRange(0, [name length])];
    return [name substringWithRange:[match rangeAtIndex:1]];
}

- (void)refreshDirectory
{
    [self cleanDirectory];
    XCTAssertTrue([self.fileManager createDirectoryAtPath:self.directory
                              withIntermediateDirectories:YES
                                               attributes:nil
                                                    error:nil]);
}

- (void)cleanDirectory
{
    if ([self.fileManager fileExistsAtPath:self.directory]) {
        XCTAssertTrue([self.fileManager removeItemAtPath:self.directory error:nil]);
    }
}

- (NSString *)className
{
    return NSStringFromClass(self.class);
}

- (NSString *)root
{
    return [NSTemporaryDirectory() stringByAppendingPathComponent:[NSBundle mainBundle].bundleIdentifier];
}

- (NSString *)directory
{
    return [self.root stringByAppendingPathComponent:self.testName];
}

- (NSFileManager *)fileManager
{
    return [NSFileManager defaultManager];
}

+ (NSString *)hint:(NSString *)description expecting:(NSString *)expected
{
    return [NSString stringWithFormat:
                     @"\nexpect [%@]"
                      "\n___but [%@]"
                      "\n__from [%@]",
                     expected,
                     description,
                     [description commonPrefixWithString:expected
                                                 options:NSCaseInsensitiveSearch]];
}

@end
