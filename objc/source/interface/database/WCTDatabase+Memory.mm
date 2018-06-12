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
#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>
#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

@implementation WCTDatabase (Memory)

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
+ (void)load
{
    NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
    //keep it the entire life cycle
    static id s_observer_memory_warning __attribute__((unused)) =
        [notificationCenter addObserverForName:
                                UIApplicationDidReceiveMemoryWarningNotification
                                        object:nil
                                         queue:nil
                                    usingBlock:^(NSNotification *) {
                                      WCDB::Database::purgeAllDatabases();
                                    }];
}
#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

- (void)purge
{
    _database->purge();
}

+ (void)purgeAllDatabases
{
    WCDB::Database::purgeAllDatabases();
}

@end
