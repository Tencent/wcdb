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

#import <WCDB/CoreConst.h>
#import <WCDB/Enum.hpp>
#import <WCDB/FileManager.hpp>
#import <WCDB/Notifier.hpp>
#import <WCDB/ThreadedErrors.hpp>
#import <WCDB/WCTCommon.h>

namespace WCDB {

#if TARGET_OS_IPHONE
template<>
constexpr const char *Enum::description(const WCDB::FileProtection &fileProtection)
{
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
}

static WCDB::FileProtection fileProtectionForAttribute(NSString *fileAttributeProtection)
{
    if ([fileAttributeProtection isEqualToString:NSFileProtectionComplete]) {
        return WCDB::FileProtection::Complete;
    } else if ([fileAttributeProtection isEqualToString:NSFileProtectionCompleteUnlessOpen]) {
        return WCDB::FileProtection::CompleteUnlessOpen;
    } else if ([fileAttributeProtection isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication]) {
        return WCDB::FileProtection::CompleteUntilFirstUserAuthentication;
    }
    return WCDB::FileProtection::None;
}

bool FileManager::setFileProtection(const WCDB::String &path, WCDB::FileProtection fileProtection)
{
    NSError *nsError = nil;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *nsPath = [NSString stringWithUTF8String:path.c_str()];
    if ([fileManager setAttributes:@{NSFileProtectionKey : @(WCDB::Enum::description(fileProtection))}
                      ofItemAtPath:nsPath
                             error:&nsError]) {
        return true;
    }
    WCDB::Error error(WCDB::Error::Code::IOError, WCDB::Error::Level::Error, nsError.description);
    error.infos.set(WCDB::ErrorStringKeySource, WCDB::ErrorSourceNative);
    error.infos.set(WCDB::ErrorStringKeyPath, path);
    error.infos.set(WCDB::ErrorIntKeyExtCode, nsError.code);
    WCDB::Notifier::shared().notify(error);
    WCDB::ThreadedErrors::shared().setThreadedError(std::move(error));
    return false;
}

std::pair<bool, WCDB::FileProtection> FileManager::getFileProtection(const WCDB::String &path)
{
    NSError *nsError = nil;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *nsPath = [NSString stringWithUTF8String:path.c_str()];
    NSDictionary *attributes = [fileManager attributesOfItemAtPath:nsPath error:&nsError];
    if (attributes != nil) {
        return { true, fileProtectionForAttribute(attributes[NSFileProtectionKey]) };
    }
    WCDB::Error error(WCDB::Error::Code::IOError, WCDB::Error::Level::Error, nsError.description);
    error.infos.set(WCDB::ErrorStringKeySource, WCDB::ErrorSourceNative);
    error.infos.set(WCDB::ErrorStringKeyPath, path);
    error.infos.set(WCDB::ErrorIntKeyExtCode, nsError.code);
    WCDB::Notifier::shared().notify(error);
    WCDB::ThreadedErrors::shared().setThreadedError(std::move(error));
    return { false, WCDB::FileProtection::None };
}
#else
bool FileManager::setFileProtection(const WCDB::String &path, WCDB::FileProtection fileProtection)
{
    WCDB_UNUSED(path)
    WCDB_UNUSED(fileProtection)
    return true;
}

std::pair<bool, WCDB::FileProtection> FileManager::getFileProtection(const WCDB::String &path)
{
    WCDB_UNUSED(path)
    return { true, WCDB::FileProtection::None };
}
#endif

}
