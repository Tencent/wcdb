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

#import "CoreConst.h"
#import "Enum.hpp"
#import "FileManager.hpp"
#import "Notifier.hpp"
#import "StringView.hpp"
#import "ThreadedErrors.hpp"
#import "WCDBError.hpp"
#import "WCTFoundation.h"
#import <Foundation/Foundation.h>

namespace WCDB {

StringView FileManager::getTemporaryDirectory()
{
    NSString *temDir = [NSTemporaryDirectory() stringByAppendingPathComponent:@"WCDBTrash"];
    return StringView(temDir.UTF8String);
}

#if TARGET_OS_IPHONE
template<>
constexpr const char *Enum::description(const FileProtection &fileProtection)
{
    switch (fileProtection) {
    case FileProtection::None:
        return NSFileProtectionNone.UTF8String;
    case FileProtection::Complete:
        return NSFileProtectionComplete.UTF8String;
    case FileProtection::CompleteUnlessOpen:
        return NSFileProtectionCompleteUnlessOpen.UTF8String;
    case FileProtection::CompleteUntilFirstUserAuthentication:
        return NSFileProtectionCompleteUntilFirstUserAuthentication.UTF8String;
    }
}

static FileProtection fileProtectionForAttribute(NSString *fileAttributeProtection)
{
    if ([fileAttributeProtection isEqualToString:NSFileProtectionComplete]) {
        return FileProtection::Complete;
    } else if ([fileAttributeProtection isEqualToString:NSFileProtectionCompleteUnlessOpen]) {
        return FileProtection::CompleteUnlessOpen;
    } else if ([fileAttributeProtection isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication]) {
        return FileProtection::CompleteUntilFirstUserAuthentication;
    }
    return FileProtection::None;
}

bool FileManager::setFileProtection(const UnsafeStringView &path, FileProtection fileProtection)
{
    NSError *nsError = nil;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *nsPath = [NSString stringWithView:path];
    if ([fileManager setAttributes:@{NSFileProtectionKey : @(Enum::description(fileProtection))}
                      ofItemAtPath:nsPath
                             error:&nsError]) {
        return true;
    }
    Error error(Error::Code::IOError, Error::Level::Error, nsError.description.UTF8String);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceNative);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    error.infos.insert_or_assign(ErrorIntKeyExtCode, nsError.code);
    Notifier::shared().notify(error);
    ThreadedErrors::shared().setThreadedError(std::move(error));
    return false;
}

Optional<FileProtection> FileManager::getFileProtection(const UnsafeStringView &path)
{
    NSError *nsError = nil;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *nsPath = [NSString stringWithView:path];
    NSDictionary *attributes = [fileManager attributesOfItemAtPath:nsPath error:&nsError];
    if (attributes != nil) {
        return fileProtectionForAttribute(attributes[NSFileProtectionKey]);
    }
    Error error(Error::Code::IOError, Error::Level::Error, nsError.description.UTF8String);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceNative);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    error.infos.insert_or_assign(ErrorIntKeyExtCode, nsError.code);
    Notifier::shared().notify(error);
    ThreadedErrors::shared().setThreadedError(std::move(error));
    return WCDB::NullOpt;
}

#else
bool FileManager::setFileProtection(const UnsafeStringView &path, FileProtection fileProtection)
{
    WCDB_UNUSED(path)
    WCDB_UNUSED(fileProtection)
    return true;
}

Optional<FileProtection> FileManager::getFileProtection(const UnsafeStringView &path)
{
    WCDB_UNUSED(path)
    return FileProtection::None;
}
#endif

}
