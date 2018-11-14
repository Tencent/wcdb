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

#import <WCDB/Notifier.hpp>
#import <WCDB/ThreadedErrors.hpp>
#import <WCDB/WCTDatabase+Memory.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTHandle+Private.h>

// Memory
#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>
#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

// FTS
#import <WCDB/WCTCursorInfo.h>
#import <WCDB/WCTDatabase+FTS.h>

@implementation WCTDatabase

+ (void)initialize
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
        NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
        //keep it the entire life cycle
        static id s_observer_memory_warning __attribute__((unused)) =
        [notificationCenter addObserverForName:
                            UIApplicationDidReceiveMemoryWarningNotification
                                        object:nil
                                         queue:nil
                                    usingBlock:^(NSNotification *) {
                                        [WCTDatabase purgeAll];
                                    }];
#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

        WCDB::Core::shared()->addTokenizer(WCTTokenizerWCDB, WCDB::FTS::Module<void, WCTCursorInfo>::address());
    });
}

- (instancetype)init
{
    return nil;
}

- (instancetype)initWithUnsafeDatabase:(WCDB::Database *)database
{
    WCTInnerAssert(database != nullptr);
    if (self = [super init]) {
        _databaseHolder = nullptr;
        _database = database;
    }
    return self;
}

- (instancetype)initWithPath:(NSString *)path
{
    if (self = [super init]) {
        _databaseHolder = WCDB::Core::shared()->getOrCreateDatabase(path);
        _database = _databaseHolder.get();
    }
    return self;
}

- (instancetype)initWithExistingPath:(NSString *)path
{
    if (self = [super init]) {
        _databaseHolder = WCDB::Core::shared()->getExistingDatabase(path);
        if (_databaseHolder == nullptr) {
            return nil;
        }
        _database = _databaseHolder.get();
    }
    return self;
}

- (instancetype)initWithExistingTag:(WCTTag)tag
{
    if (self = [super init]) {
        _databaseHolder = WCDB::Core::shared()->getExistingDatabase(tag);
        if (_databaseHolder == nullptr) {
            return nil;
        }
        _database = _databaseHolder.get();
    }
    return self;
}

- (void)invalidate
{
    _databaseHolder = nullptr;
    _database = nullptr;
}

- (BOOL)isValidated
{
    return _database != nullptr;
}

- (void)setTag:(WCTTag)tag
{
    WCTDatabaseAssert(return;);
    _database->setTag(tag);
}

- (WCTTag)tag
{
    WCTDatabaseAssert(return WCTInvalidTag;);
    return _database->getTag();
}

- (NSString *)path
{
    WCTDatabaseAssert(return nil;);
    return [NSString stringWithUTF8String:_database->getPath().c_str()];
}

- (BOOL)canOpen
{
    WCTDatabaseAssert(return NO;);
    return _database->canOpen();
}

- (BOOL)isOpened
{
    WCTDatabaseAssert(return NO;);
    return _database->isOpened();
}

- (void)close
{
    WCTDatabaseAssert(return;);
    _database->close(nullptr);
}

- (void)close:(WCDB_NO_ESCAPE WCTCloseBlock)onClosed
{
    WCTDatabaseAssert(return;);
    std::function<void(void)> callback = nullptr;
    if (onClosed) {
        callback = [onClosed]() {
            onClosed();
        };
    }
    _database->close(callback);
}

- (BOOL)isBlockaded
{
    WCTDatabaseAssert(return NO;);
    return _database->isBlockaded();
}

- (void)blockade
{
    WCTDatabaseAssert(return;);
    _database->blockade();
}

- (void)unblockade
{
    WCTDatabaseAssert(return;);
    _database->unblockade();
}

- (WCTError *)error
{
    WCTDatabaseAssert(return nil;);
    return [[WCTError alloc] initWithError:_database->getThreadedError()];
}

- (WCDB::RecyclableHandle)generateHandle
{
    return _database->getHandle();
}

@end
