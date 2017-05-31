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

#import <WCDB/WCTDatabase+File.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/utility.hpp>

@implementation WCTDatabase (File)

- (BOOL)removeFilesWithError:(WCTError **)error
{
    WCDB::Error unixError;
    bool result = _database->removeFiles(unixError);
    if (!result && error) {
        *error = [WCTError errorWithWCDBError:unixError];
    }
    return result;
}

- (BOOL)moveFilesToDirectory:(NSString *)directory withExtraFiles:(NSArray<NSString *> *)extraFiles andError:(WCTError **)error
{
    WCDB::Error unixError;
    std::list<std::string> files;
    for (NSString *extraFile in extraFiles) {
        files.push_back(extraFile.UTF8String);
    }
    bool result = _database->moveFilesToDirectoryWithExtraFiles(directory.UTF8String, files, unixError);
    if (!result && error) {
        *error = [WCTError errorWithWCDBError:unixError];
    }
    return result;
}

- (BOOL)moveFilesToDirectory:(NSString *)directory withError:(WCTError **)error
{
    WCDB::Error unixError;
    bool result = _database->moveFiles(directory.UTF8String, unixError);
    if (!result && error) {
        *error = [WCTError errorWithWCDBError:unixError];
    }
    return result;
}

- (NSArray<NSString *> *)getPaths
{
    NSMutableArray *paths = [NSMutableArray array];
    for (const auto &path : _database->getPaths()) {
        [paths addObject:@(path.c_str())];
    }
    return paths;
}

- (NSUInteger)getFilesSizeWithError:(WCTError **)error
{
    WCDB::Error unixError;
    size_t size = _database->getFilesSize(unixError);
    if (!unixError.isOK() && error) {
        *error = [WCTError errorWithWCDBError:unixError];
    }
    return size;
}

@end
