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
#import <WCDB/Notifier.hpp>
#import <WCDB/ThreadedErrors.hpp>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

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
    if (self == WCTDatabase.class) {
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
    }
}

- (instancetype)initWithPath:(NSString *)path
{
    return [super initWithHolder:WCDB::Core::shared()->getOrCreateDatabase(path)];
}

- (instancetype)initWithExistingPath:(NSString *)path
{
    return [super initWithHolder:WCDB::Core::shared()->getExistingDatabase(path)];
}

- (instancetype)initWithExistingTag:(WCTTag)tag
{
    return [super initWithHolder:WCDB::Core::shared()->getExistingDatabase(tag)];
}

- (void)setTag:(WCTTag)tag
{
    _database->setTag(tag);
}

- (BOOL)canOpen
{
    return _database->canOpen();
}

- (BOOL)isOpened
{
    return _database->isOpened();
}

- (void)close
{
    _database->close(nullptr);
}

- (void)close:(WCDB_NO_ESCAPE WCTCloseBlock)onClosed
{
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
    return _database->isBlockaded();
}

- (void)blockade
{
    _database->blockade();
}

- (void)unblockade
{
    _database->unblockade();
}

- (WCTError *)error
{
    return [[WCTError alloc] initWithError:_database->getThreadedError()];
}

@end
