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
#import <WCDB/WCTDatabase+FTS.h>
#import <WCDB/WCTOneOrBinaryTokenizer.h>

namespace WCDB {
static constexpr const char *NotifierPathPreprocessorName
= "com.Tencent.WCDB.Notifier.PreprocessPath";
}

@implementation WCTDatabase

+ (void)initialize
{
    if (self.class == WCTDatabase.class) {
        WCDB::Notifier::shared()->setNotificationForPreprocessing(WCDB::NotifierPathPreprocessorName, [](const WCDB::Error &error, WCDB::Error::Infos &infos, WCDB::Error::Level &newLevel) {
            const auto &strings = error.infos.getStrings();
            auto iter = strings.find(WCTErrorKeyPath);
            if (iter == strings.end()) {
                return;
            }
            NSString *path = [NSString stringWithUTF8String:iter->second.c_str()].stringByAbbreviatingWithTildeInPath;
            if (path.length > 0) {
                infos.set(WCTErrorKeyPath, path.UTF8String);
            }
        });

        WCDB::Console::shared()->setPrinter([](const WCDB::String &message) {
            NSLog(@"%s", message.c_str());
        });

        WCDB::Core::shared()->addTokenizer(WCTTokenizerOneOrBinary, WCDB::TokenizerModuleTemplate<WCDB::OneOrBinaryTokenizerInfo, WCTOneOrBinaryTokenizerCursorInfo>::specialize());

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
    }
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
        WCTInnerAssert(_databaseHolder != nullptr);
        _database = _databaseHolder.get();
        WCTInnerAssert(_database != nullptr);
    }
    return self;
}

- (instancetype)initWithPathOfAlivingDatabase:(NSString *)path
{
    if (self = [super init]) {
        _databaseHolder = WCDB::Core::shared()->getAlivingDatabase(path);
        if (_databaseHolder == nullptr) {
            return nil;
        }
        _database = _databaseHolder.get();
        WCTInnerAssert(_database != nullptr);
    }
    return self;
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
    return [NSString stringWithUTF8String:_database->getPath().c_str()];
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
    if (onClosed != nil) {
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

- (WCDB::RecyclableHandle)generateHandle
{
    return _database->getHandle();
}

@end
