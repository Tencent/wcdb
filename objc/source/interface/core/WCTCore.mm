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
#import <WCDB/WCTCore+Private.h>

@implementation WCTCore

- (instancetype)init
{
    return [self initWithDatabase:nullptr];
}

- (instancetype)initWithHolder:(const WCDB::RecyclableDatabase &)database
{
    WCTInnerAssert(database != nullptr);
    if (self = [super init]) {
        _holder = database;
        _database = database.get();
    }
    return self;
}

- (instancetype)initWithDatabase:(WCDB::Database *)database
{
    if (database == nullptr) {
        return nil;
    }
    if (self = [super init]) {
        _database = database;
    }
    return self;
}

- (instancetype)initWithCore:(WCTCore *)core
{
    WCTInnerAssert(core != nil);
    if (self = [super init]) {
        _holder = core->_holder;
        _database = core->_database;
    }
    return self;
}

- (WCTTag)tag
{
    return _database->getTag();
}

- (NSString *)path
{
    return [NSString stringWithCppString:_database->getPath()];
}

- (void)finalizeDatabase
{
    _holder = nullptr;
    _database = nullptr;
}

@end
