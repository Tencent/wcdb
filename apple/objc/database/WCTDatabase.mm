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

#import <WCDB/Global.hpp>
#import <WCDB/ThreadedErrors.hpp>
#import <WCDB/WCTDatabase+FTS.h>
#import <WCDB/WCTDatabase+Memory.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTFoundation.h>
#import <WCDB/WCTHandle+Private.h>

@implementation WCTDatabase

- (instancetype)initWithUnsafeDatabase:(WCDB::Database *)database
{
    WCTAssert(database != nullptr);
    if (self = [super init]) {
        _databaseHolder = nullptr;
        _database = database;
    }
    return self;
}

- (instancetype)initWithPath:(NSString *)path
{
    if (self = [super init]) {
        path = [path wcdb_stringByStandardizingPath];
        _databaseHolder = WCDB::Core::shared().getOrCreateDatabase(path);
        WCTAssert(_databaseHolder != nullptr);
        _database = _databaseHolder.get();
        WCTAssert(_database != nullptr);
    }
    return self;
}

- (instancetype)initInMemoryDatabase
{
    if (self = [super init]) {
        m_isInMemory = YES;
        _database = new WCDB::Database(":memory:");
        _database->setInMemory();
        WCTAssert(_database != nullptr);
    }
    return self;
}

- (void)dealloc
{
    if (m_isInMemory) {
        delete _database;
    }
}

- (void)setTag:(WCTTag)tag
{
    _database->setTag(tag);
}

- (WCTTag)tag
{
    return _database->getTag();
}

- (NSString *)path
{
    return [NSString stringWithView:_database->getPath()];
}

- (void)close:(WCDB_NO_ESCAPE WCTCloseBlock)onClosed
{
    std::function<void(void)> callback = nullptr;
    if (onClosed != nil) {
        callback = [onClosed]() {
            onClosed();
        };
    }
    _database->close(callback);
}

- (BOOL)truncateCheckpoint
{
    return _database->checkpoint(false, WCDB::Database::CheckPointMode::Truncate);
}

- (BOOL)passiveCheckpoint
{
    return _database->checkpoint(false, WCDB::Database::CheckPointMode::Passive);
}

- (WCTError *)error
{
    return [[WCTError alloc] initWithError:_database->getThreadedError()];
}

- (WCDB::RecyclableHandle)generateHandle
{
    return _database->getHandle();
}

@end
