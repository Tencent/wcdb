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

#import <Foundation/Foundation.h>
#import <WCDB/Macro.hpp>
#import <WCDB/ObservationQueue.hpp>

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>

namespace WCDB {

void *ObservationQueue::registerNotificationWhenMemoryWarning()
{
    id observer = [[NSNotificationCenter defaultCenter]
    addObserverForName:UIApplicationDidReceiveMemoryWarningNotification
                object:nil
                 queue:nil
            usingBlock:^(NSNotification *) {
                this->observatedThatNeedPurged();
            }];
    return (void *) CFBridgingRetain(observer);
}

void ObservationQueue::unregisterNotificationWhenMemoryWarning(void *observer)
{
    NSObject *nsObserver = (__bridge NSObject *) observer;
    [[NSNotificationCenter defaultCenter] removeObserver:nsObserver name:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    CFBridgingRelease(observer);
}

}
#else // TARGET_OS_IPHONE && !TARGET_OS_WATCH

namespace WCDB {

void* ObservationQueue::registerNotificationWhenMemoryWarning()
{
    return nullptr;
}

void ObservationQueue::unregisterNotificationWhenMemoryWarning(void* observer)
{
    WCDB_UNUSED(observer)
}

}

#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH
