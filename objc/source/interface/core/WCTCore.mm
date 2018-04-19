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
#import <WCDB/NSData+NoCopyData.h>
#import <WCDB/WCTCore+Private.h>

@implementation WCTCore

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
{
    if (database == nullptr) {
        return nil;
    }
    if (self = [self init]) {
        _database = database;
    }
    return self;
}

- (const std::shared_ptr<WCDB::Database> &)database
{
    return _database;
}

- (WCTTag)tag
{
    return _database->getTag();
}

- (NSString *)path
{
    return @(_database->getPath().c_str());
}

- (void)finalizeDatabase
{
    _database = nullptr;
}

- (nullable WCTCoreError *)error
{
    return nil;
}

@end
