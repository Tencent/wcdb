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

#import <Foundation/Foundation.h>
#import <WCDB/FileManager.hpp>
#import <WCDB/NSString+cppString.h>
#import <WCDB/Notifier.hpp>
#import <WCDB/ThreadedErrors.hpp>

namespace WCDB {

bool FileManager::setFileProtection(const std::string &path, WCDB::FileProtection fileProtection)
{
#if TARGET_OS_IPHONE
    NSError *nsError = nil;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *nsPath = [NSString stringWithUTF8String:path.c_str()];
    if ([fileManager setAttributes:@{NSFileProtectionKey : @(FileManager::fileProtectionName(fileProtection))}
                      ofItemAtPath:nsPath
                             error:&nsError]) {
        return true;
    }
    WCDB::Error error;
    error.setCode(WCDB::Error::Code::IOError, "Native");
    if (nsError.description.length > 0) {
        error.message = nsError.description.cppString;
    } else {
        error.message = WCDB::Error::codeName(WCDB::Error::Code::IOError);
    }
    error.infos.set("Path", path);
    error.infos.set("ExtCode", nsError.code);
    WCDB::Notifier::shared()->notify(error);
    WCDB::ThreadedErrors::shared()->setThreadedError(std::move(error));
    return false;
#else
    return true;
#endif
}

std::pair<bool, WCDB::FileProtection> FileManager::getFileProtection(const std::string &path)
{
#if TARGET_OS_IPHONE
    NSError *nsError = nil;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *nsPath = [NSString stringWithUTF8String:path.c_str()];
    NSDictionary *attributes = [fileManager attributesOfItemAtPath:nsPath error:&nsError];
    if (attributes) {
        WCDB::FileProtection fileProtection = WCDB::FileProtection::None;
        NSString *nsFileProtection = [attributes objectForKey:NSFileProtectionKey];
        if ([nsFileProtection isEqualToString:NSFileProtectionComplete]) {
            fileProtection = WCDB::FileProtection::Complete;
        } else if ([nsFileProtection isEqualToString:NSFileProtectionCompleteUnlessOpen]) {
            fileProtection = WCDB::FileProtection::CompleteUnlessOpen;
        } else if ([nsFileProtection isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication]) {
            fileProtection = WCDB::FileProtection::CompleteUntilFirstUserAuthentication;
        }
        return { true, fileProtection };
    }
    WCDB::Error error;
    error.setCode(WCDB::Error::Code::IOError, "Native");
    if (nsError.description.length > 0) {
        error.message = nsError.description.cppString;
    } else {
        error.message = WCDB::Error::codeName(WCDB::Error::Code::IOError);
    }
    error.infos.set("Path", path);
    error.infos.set("ExtCode", nsError.code);
    WCDB::Notifier::shared()->notify(error);
    WCDB::ThreadedErrors::shared()->setThreadedError(std::move(error));
    return { false, WCDB::FileProtection::None };
#else
    return { true, std::numeric_limits<WCDB::FileProtection>::max() };
#endif
}

const char *FileManager::fileProtectionName(WCDB::FileProtection fileProtection)
{
#if TARGET_OS_IPHONE
    switch (fileProtection) {
    case WCDB::FileProtection::None:
        return NSFileProtectionNone.UTF8String;
    case WCDB::FileProtection::Complete:
        return NSFileProtectionComplete.UTF8String;
    case WCDB::FileProtection::CompleteUnlessOpen:
        return NSFileProtectionCompleteUnlessOpen.UTF8String;
    case WCDB::FileProtection::CompleteUntilFirstUserAuthentication:
        return NSFileProtectionCompleteUntilFirstUserAuthentication.UTF8String;
    }
#else
    return "";
#endif
}

}
