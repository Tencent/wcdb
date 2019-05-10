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
#import <WCDB/WCTConvertible.h>
#import <WCDB/WCTDatabase+Debug.h>
#import <WCDB/WCTDatabase+Monitor.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTError.h>

NSErrorDomain const WCTErrorDomain = @"WCDB";

NSErrorUserInfoKey const WCTErrorKeyPath = @WCDB_ERROR_STRING_KEY_PATH;
NSErrorUserInfoKey const WCTErrorKeySQL = @WCDB_ERROR_STRING_KEY_SQL;
NSErrorUserInfoKey const WCTErrorKeyTag = @WCDB_ERROR_INT_KEY_TAG;
NSErrorUserInfoKey const WCTErrorKeyExtendedCode = @WCDB_ERROR_INT_KEY_EXTCODE;
NSErrorUserInfoKey const WCTErrorKeySource = @WCDB_ERROR_STRING_KEY_SOURCE;

@implementation WCTError

- (instancetype)initWithCode:(WCTErrorCode)code
                       level:(WCTErrorLevel)level
                     message:(NSString *)message
                    userInfo:(NSDictionary<NSErrorUserInfoKey, id> *)userInfo
{
    if (self = [super initWithDomain:WCTErrorDomain
                                code:(NSInteger) code
                            userInfo:userInfo]) {
        WCTInnerAssert(message.length > 0);
        _message = message;
        _level = (WCTErrorLevel) level;
    }
    return self;
}

- (instancetype)initWithError:(const WCDB::Error &)error
{
    NSMutableDictionary *userInfo = [[NSMutableDictionary alloc] init];
    for (const auto &info : error.infos.getIntegers()) {
        [userInfo setObject:[NSNumber numberWithLongLong:info.second] forKey:[NSString stringWithUTF8String:info.first.c_str()]];
    }
    for (const auto &info : error.infos.getStrings()) {
        [userInfo setObject:[NSString stringWithUTF8String:info.second.c_str()]
                     forKey:[NSString stringWithUTF8String:info.first.c_str()]];
    }
    for (const auto &info : error.infos.getDoubles()) {
        [userInfo setObject:[NSNumber numberWithDouble:info.second] forKey:[NSString stringWithUTF8String:info.first.c_str()]];
    }
    return [self initWithCode:(WCTErrorCode) error.code() level:(WCTErrorLevel) error.level message:[NSString stringWithUTF8String:error.getMessage().c_str()] userInfo:userInfo];
}

- (BOOL)isOK
{
    return self.code == WCTErrorCodeOK;
}

- (BOOL)isCorruption
{
    return self.code == WCTErrorCodeCorrupt || self.code == WCTErrorCodeNotADatabase;
}

- (NSString *)stringForKey:(NSErrorUserInfoKey)key
{
    id value = [self.userInfo objectForKey:key];
    if (value != nil && [value isKindOfClass:NSString.class]) {
        return value;
    }
    return nil;
}

- (NSNumber *)numberForKey:(NSErrorUserInfoKey)key
{
    id value = [self.userInfo objectForKey:key];
    if (value != nil && [value isKindOfClass:NSNumber.class]) {
        return value;
    }
    return nil;
}

- (NSString *)description
{
    NSMutableString *description = nil;
    if (self.code != WCTErrorCodeOK
        && self.code != WCTErrorCodeRow
        && self.code != WCTErrorCodeDone) {
        description = [[NSMutableString alloc] initWithFormat:@"[%s: %ld, %@]", WCDB::Error::levelName((WCDB::Error::Level) self.level), (long) self.code, self.message];
        [self.userInfo enumerateKeysAndObjectsUsingBlock:^(NSErrorUserInfoKey key, id obj, BOOL *) {
            [description appendFormat:@", %@: %@", key, obj];
        }];
    }
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
    static_assert(WCTInvalidTag == 0, "");
    return (WCTTag) [self numberForKey:WCTErrorKeyTag].intValue;
}

@end

@implementation WCTError (SQL)

- (NSString *)sql
{
    return [self stringForKey:WCTErrorKeySQL];
}

@end

@implementation WCTError (ExtendedCode)

- (WCTErrorExtendedCode)extendedCode
{
    return (WCTErrorExtendedCode) [self numberForKey:WCTErrorKeyExtendedCode].intValue;
}

@end

@implementation WCTError (Source)

- (NSString *)source
{
    return [self stringForKey:WCTErrorKeySource];
}

@end
