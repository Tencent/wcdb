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

typedef NSString *WCTErrorKey;
WCTErrorKey const WCTErrorKeyPath = @"Path";
WCTErrorKey const WCTErrorKeySQL = @"SQL";
WCTErrorKey const WCTErrorKeyOperation = @"Op";
WCTErrorKey const WCTErrorKeyTag = @"Tag";
WCTErrorKey const WCTErrorKeyExtendedCode = @"ExtCode";
WCTErrorKey const WCTErrorKeySource = @"Source";

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
                                code:(NSInteger) error.code()
                            userInfo:userInfo]) {
        if (!error.message.empty()) {
            _message = [NSString stringWithCppString:error.message];
        } else {
            _message = [NSString stringWithUTF8String:WCDB::Error::codeName((WCDB::Error::Code) self.code)];
        }
        _level = (WCTErrorLevel) error.level;
    }
    return self;
}

- (BOOL)isOK
{
    return self.code == WCTErrorCodeOK;
}

- (BOOL)isCorruption
{
    return self.code == WCTErrorCodeCorrupt || self.code == WCTErrorCodeNotADatabase;
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
    return 0;
}

- (NSString *)description
{
    if (self.code == WCTErrorCodeOK) {
        return nil;
    }
    NSMutableString *description = [[NSMutableString alloc] initWithFormat:@"[%s: %ld, %@]", WCDB::Error::levelName((WCDB::Error::Level) self.level), (long) self.code, self.message];
    [self.userInfo enumerateKeysAndObjectsUsingBlock:^(NSErrorUserInfoKey key, id obj, BOOL *) {
      [description appendFormat:@", %@: %@", key, obj];
    }];
    return description;
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

@implementation WCTError (ExtendedCode)

- (NSInteger)extendedCode
{
    return (NSInteger) [self integerForKey:WCTErrorKeyExtendedCode];
}

@end

@implementation WCTError (Source)

- (NSString *)source
{
    return [self stringForKey:WCTErrorKeySource];
}

@end
