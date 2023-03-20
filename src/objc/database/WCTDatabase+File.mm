//
// Created by sanhuazhang on 2019/05/02
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

#import <WCDB/WCTDatabase+File.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTFoundation.h>

@implementation WCTDatabase (File)

- (BOOL)removeFiles
{
    return _database->removeFiles();
}

- (BOOL)moveFilesToDirectory:(NSString *)directory
{
    return _database->moveFiles(directory);
}

- (NSArray<NSString *> *)paths
{
    const auto &paths = _database->getPaths();
    NSMutableArray *nsPaths = [NSMutableArray arrayWithCapacity:paths.size()];
    for (const auto &path : paths) {
        [nsPaths addObject:[NSString stringWithView:path]];
    }
    return nsPaths;
}

- (WCTOptionalSize)getFilesSize
{
    return _database->getFilesSize();
}

+ (BOOL)setDefaultTemporaryDatabaseFileDirectory:(NSString *)dir
{
    if (dir.length == 0) {
        WCDB::Error error(WCDB::Error::Code::Error,
                          WCDB::Error::Level::Error,
                          "Set nil temporary database files directory!");
        WCDB::Notifier::shared().notify(error);
        return NO;
    }
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL isDir = FALSE;
    BOOL isDirExist = [fileManager fileExistsAtPath:dir
                                        isDirectory:&isDir];
    if (!isDirExist) {
        BOOL bCreateDir = [fileManager createDirectoryAtPath:dir
                                 withIntermediateDirectories:YES
                                                  attributes:nil
                                                       error:nil];
        if (!bCreateDir) {
            WCDB::Error error(WCDB::Error::Code::Error,
                              WCDB::Error::Level::Error,
                              "Fail to create temporary database files directory");
            error.infos.insert_or_assign("Path", dir.UTF8String);
            WCDB::Notifier::shared().notify(error);
            return NO;
        }
    } else if (!isDir) {
        WCDB::Error error(WCDB::Error::Code::Error,
                          WCDB::Error::Level::Error,
                          "Invalid directory");
        error.infos.insert_or_assign("Path", dir.UTF8String);
        WCDB::Notifier::shared().notify(error);
        return NO;
    }

    setenv("SQLITE_TMPDIR", dir.UTF8String, 1);
    return YES;
}

@end
