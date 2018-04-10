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

#import <WCDB/Interface.h>
#import <WCDB/WCTError+Private.h>

@implementation WCTCoreError

- (instancetype)initWithWCDBError:(const WCDB::Error *)error
{
    if (self = [super initWithWCDBError:error]) {
        assert(error->getHashedTypeid() == typeid(WCDB::CoreError).hash_code());
        const WCDB::CoreError *coreError = static_cast<const WCDB::CoreError *>(error);
        _tag = coreError->tag;
        _path = @(coreError->path.c_str());
    }
    return self;
}

- (WCTErrorType)type
{
    return WCTErrorTypeCore;
}

- (NSString *)description
{
    NSMutableString *desc = [[NSMutableString alloc] initWithString:[super description]];
    [desc appendFormat:@"Tag: %d", _tag];
    [desc appendFormat:@"Path: %@", _path];
    return desc;
}

@end
