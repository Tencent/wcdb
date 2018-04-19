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

#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTFileError+Private.h>

@implementation WCTDatabase (File)

- (BOOL)removeFiles
{
    return _database->removeFiles();
}

- (BOOL)moveFilesToDirectory:(NSString *)directory
              withExtraFiles:(NSArray<NSString *> *)extraFiles
{
    std::list<std::string> files;
    for (NSString *extraFile in extraFiles) {
        files.push_back(extraFile.UTF8String);
    }
    return _database->moveFilesToDirectoryWithExtraFiles(directory.UTF8String, files);
}

- (BOOL)moveFilesToDirectory:(NSString *)directory
{
    return _database->moveFiles(directory.UTF8String);
}

- (NSArray<NSString *> *)paths
{
    NSMutableArray *paths = [NSMutableArray array];
    for (const auto &path : _database->getPaths()) {
        [paths addObject:@(path.c_str())];
    }
    return paths;
}

- (NSUInteger)getFilesSize
{
    return _database->getFilesSize().second;
}

- (WCTFileError *)getFileError
{
    return [[WCTFileError alloc] initWithFileError:_database->getFileError()];
}

@end
