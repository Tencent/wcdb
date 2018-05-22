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

#import <WCDB/FileManager.hpp>
#import <WCDB/Interface.h>
#import <WCDB/NSString+CppString.h>

WCTErrorType const WCTErrorTypeSQLite = @"SQLite";
WCTErrorType const WCTErrorTypeHandle = @"Handle";
WCTErrorType const WCTErrorTypeCore = @"Core";
WCTErrorType const WCTErrorTypeFileManager = @"FileManager";

WCTErrorKey const WCTErrorKeyPath = @"Path";
WCTErrorKey const WCTErrorKeySQL = @"SQL";
WCTErrorKey const WCTErrorKeyFileOperation = @"Op";
WCTErrorKey const WCTErrorKeyTag = @"Tag";

WCTTag const WCTInvalidTag = WCDB::Handle::invalidTag;

static_assert((int) WCTErrorLevelIgnore == (int) WCDB::Error::Level::Ignore, "");
static_assert((int) WCTErrorLevelDebug == (int) WCDB::Error::Level::Debug, "");
static_assert((int) WCTErrorLevelWarning == (int) WCDB::Error::Level::Warning, "");
static_assert((int) WCTErrorLevelError == (int) WCDB::Error::Level::Error, "");
static_assert((int) WCTErrorLevelFatal == (int) WCDB::Error::Level::Fatal, "");

static_assert((int) WCTInvalidTag == (int) WCDB::Handle::invalidTag, "");
static_assert(sizeof(WCTTag) == sizeof(WCDB::Handle::Tag), "");

static_assert((int) WCTErrorFileOperationNotSet == (int) WCDB::FileManager::Operation::NotSet, "");
static_assert((int) WCTErrorFileOperationLstat == (int) WCDB::FileManager::Operation::Lstat, "");
static_assert((int) WCTErrorFileOperationAccess == (int) WCDB::FileManager::Operation::Access, "");
static_assert((int) WCTErrorFileOperationLink == (int) WCDB::FileManager::Operation::Link, "");
static_assert((int) WCTErrorFileOperationUnlink == (int) WCDB::FileManager::Operation::Unlink, "");
static_assert((int) WCTErrorFileOperationRemove == (int) WCDB::FileManager::Operation::Remove, "");
static_assert((int) WCTErrorFileOperationMkdir == (int) WCDB::FileManager::Operation::Mkdir, "");

@implementation WCTError

- (instancetype)initWithError:(const WCDB::Error &)error
{
    NSMutableDictionary *userInfo = [[NSMutableDictionary alloc] init];
    for (const auto &info : error.infos.getIntInfos()) {
        [userInfo setObject:@(info.second) forKey:[NSString stringWithCppString:info.first]];
    }
    for (const auto &info : error.infos.getStringInfos()) {
        [userInfo setObject:[NSString stringWithCppString:info.second] forKey:[NSString stringWithCppString:info.first]];
    }

    if (self = [super initWithDomain:@"WCDB"
                                code:error.code
                            userInfo:userInfo]) {
        _message = [NSString stringWithCppString:error.message];
        _level = (WCTErrorLevel) error.level;
        _type = [NSString stringWithCppString:error.type];
        WCTInnerAssert([_type isEqualToString:WCTErrorTypeCore] || [_type isEqualToString:WCTErrorTypeSQLite] || [_type isEqualToString:WCTErrorTypeHandle] || [_type isEqualToString:WCTErrorTypeFileManager]);
    }
    return self;
}

- (NSString *)stringForKey:(WCTErrorKey)key
{
    id value = [self.userInfo objectForKey:key];
    if (value && [value isKindOfClass:NSString.class]) {
        return value;
    }
    return nil;
}

- (NSInteger)integerForKey:(WCTErrorKey)key
{
    id value = [self.userInfo objectForKey:WCTErrorKeyTag];
    if (value && [value isKindOfClass:NSNumber.class]) {
        return ((NSNumber *) value).integerValue;
    }
    if ([key isEqualToString:WCTErrorKeyTag]) {
        return WCTInvalidTag;
    }
    if ([key isEqualToString:WCTErrorKeyFileOperation]) {
        return WCTErrorFileOperationNotSet;
    }
    return 0;
}

- (NSString *)description
{
    NSMutableString *description = [[NSMutableString alloc] initWithFormat:@"[%s]", WCDB::Error::LevelName((WCDB::Error::Level) self.level)];
    WCTInnerAssert(self.type.length > 0);
    [description appendString:self.type];
    if (self.code != WCDB::Error::error) {
        [description appendFormat:@", Code: %ld", (long) self.code];
    }
    if (self.message.length > 0) {
        [description appendFormat:@", Msg: %@", self.message];
    }
    [self.userInfo enumerateKeysAndObjectsUsingBlock:^(NSErrorUserInfoKey key, id obj, BOOL *) {
      [description appendFormat:@", %@: %@", key, obj];
    }];
    return description;
}

@end

@implementation WCTError (File)

- (WCTErrorFileOperation)fileOperation
{
    return (WCTErrorFileOperation) [self integerForKey:WCTErrorKeyFileOperation];
}

@end

@implementation WCTError (Path)

- (NSString *)path
{
    return [self stringForKey:WCTErrorKeyPath];
}

@end

@implementation WCTError (Tag)

- (WCTTag)tag
{
    return (WCTTag) [self integerForKey:WCTErrorKeyTag];
}

@end

@implementation WCTError (SQL)

- (NSString *)sql
{
    return [self stringForKey:WCTErrorKeySQL];
}

@end
