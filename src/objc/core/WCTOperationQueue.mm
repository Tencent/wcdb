//
// Created by sanhuazhang on 2019/05/27
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

#import "OperationQueueForMemory.hpp"
#import <Foundation/Foundation.h>

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH && !TARGET_OS_TV
#import <UIKit/UIKit.h>

namespace WCDB {

void *OperationQueueForMemory::registerNotificationWhenMemoryWarning()
{
    id observer = [[NSNotificationCenter defaultCenter]
    addObserverForName:UIApplicationDidReceiveMemoryWarningNotification
                object:nil
                 queue:nil
            usingBlock:^(NSNotification *) {
                this->asyncPurgeWhenMemoryWarning();
            }];
    return (void *) CFBridgingRetain(observer);
}

void OperationQueueForMemory::unregisterNotificationWhenMemoryWarning(void *observer)
{
    NSObject *nsObserver = (__bridge NSObject *) observer;
    [[NSNotificationCenter defaultCenter] removeObserver:nsObserver name:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    CFBridgingRelease(observer);
}

}
#else // TARGET_OS_IPHONE && !TARGET_OS_WATCH

namespace WCDB {

void *OperationQueueForMemory::registerNotificationWhenMemoryWarning()
{
    return nullptr;
}

void OperationQueueForMemory::unregisterNotificationWhenMemoryWarning(void *)
{
}

}

#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

namespace WCDB {

void OperationQueueForMemory::executeOperationWithAutoMemoryRelease(std::function<void(void)> operation)
{
    @autoreleasepool {
        operation();
    }
}

}
