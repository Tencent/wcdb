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
        switch (iter.second.getType()) {
            case WCDB::ErrorValue::Type::Int:
                [infos setObject:@(iter.second.getIntValue()) forKey:@((int) iter.first)];
                break;
            case WCDB::ErrorValue::Type::String: {
                NSString *value;
                if (iter.first != WCDB::Error::Key::Path) {
                    value = @(iter.second.getStringValue().c_str());
                } else {
                    value = [@(iter.second.getStringValue().c_str()) stringByAbbreviatingWithTildeInPath];
                }
                [infos setObject:value forKey:@((int) iter.first)];
            } break;
        }
    }
    if (self = [super initWithDomain:@"WCDB" code:error.getCode() userInfo:infos]) {
        _type = (WCTErrorType) error.getType();
    }
    return self;
}

- (BOOL)isOK
{
    return _type == 0;
}

- (NSString *)description
{
    NSMutableString *desc = [[NSMutableString alloc] init];
    [desc appendFormat:@"Code:%ld, ", (long) self.code];
    [desc appendFormat:@"Type:%s, ", WCDB::Error::GetTypeName((WCDB::Error::Type) _type)];
    __block BOOL quote = NO;
    [self.userInfo enumerateKeysAndObjectsUsingBlock:^(NSNumber *key, id obj, BOOL *_Nonnull unused) {
      if (quote) {
          [desc appendString:@", "];
      } else {
          quote = YES;
      }
      [desc appendFormat:@"%s:%@", WCDB::Error::GetKeyName((WCDB::Error::Key) key.intValue), obj];
    }];
    return desc;
}

- (id)infoForKey:(WCTErrorKey)key
{
    return [self.userInfo objectForKey:@(key)];
}

@end
