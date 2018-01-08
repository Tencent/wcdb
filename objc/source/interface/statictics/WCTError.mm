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

#import <WCDB/WCTError.h>

@implementation WCTError

+ (instancetype)errorWithWCDBError:(const WCDB::Error &)error
{
    return [[WCTError alloc] initWithWCDBError:error];
}

- (instancetype)initWithWCDBError:(const WCDB::Error &)error
{
    NSMutableDictionary *infos = [NSMutableDictionary dictionary];
    for (const auto &iter : error.getInfos()) {
        NSString *key = @(WCDB::Error::GetKeyName(iter.first));
        switch (iter.second.getType()) {
            case WCDB::ErrorValue::Type::Int:
                [infos setObject:@(iter.second.getIntValue()) forKey:key];
                break;
            case WCDB::ErrorValue::Type::String: {
                const std::string stringValue = iter.second.getStringValue();
                NSString *value = @(stringValue.c_str());
                if (!value) {
                    value = [[NSString alloc] initWithCString:stringValue.c_str() encoding:NSASCIIStringEncoding];
                }
                if (!value) {
                    value = @"";
                }
                [infos setObject:value forKey:key];
            } break;
        }
    }
    return [self initWithType:(WCTErrorType) error.getType() code:error.getCode() userInfo:infos];
}

- (instancetype)initWithType:(WCTErrorType)type code:(NSInteger)code userInfo:(NSDictionary *)userInfo
{
    if (self = [super initWithDomain:@"WCDB" code:code userInfo:userInfo]) {
        _type = type;
    }
    return self;
}

- (BOOL)isOK
{
    return self.code == 0;
}

- (NSString *)description
{
    NSMutableString *desc = [[NSMutableString alloc] init];
    [desc appendFormat:@"Code:%ld, ", (long) self.code];
    [desc appendFormat:@"Type:%s, ", WCDB::Error::GetTypeName((WCDB::Error::Type) _type)];
    __block BOOL quote = NO;
    [self.userInfo enumerateKeysAndObjectsUsingBlock:^(NSString *key, id obj, BOOL *_Nonnull unused) {
      if (quote) {
          [desc appendString:@", "];
      } else {
          quote = YES;
      }
      [desc appendFormat:@"%@:%@", key, obj];
    }];
    return desc;
}

- (id)infoForKey:(WCTErrorKey)key
{
    return [self.userInfo objectForKey:@(WCDB::Error::GetKeyName((WCDB::Error::Key) key))];
}

@end
