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

#import <Foundation/Foundation.h>
#import <WCDB/Macro.h>
#import <WCDB/OperationQueue.hpp>

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>

namespace WCDB {

void *OperationQueue::registerNotificationWhenMemoryWarning()
{
    id observer = [[NSNotificationCenter defaultCenter]
    addObserverForName:UIApplicationDidReceiveMemoryWarningNotification
                object:nil
                 queue:nil
            usingBlock:^(NSNotification *) {
                Parameter parameter;
                parameter.source = Parameter::Source::MemoryWarning;
                this->asyncPurge(parameter);
            }];
    return (void *) CFBridgingRetain(observer);
}

void OperationQueue::unregisterNotificationWhenMemoryWarning(void *observer)
{
    NSObject *nsObserver = (__bridge NSObject *) observer;
    [[NSNotificationCenter defaultCenter] removeObserver:nsObserver name:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    CFBridgingRelease(observer);
}

}
#else // TARGET_OS_IPHONE && !TARGET_OS_WATCH

namespace WCDB {

void* OperationQueue::registerNotificationWhenMemoryWarning()
{
    return nullptr;
}

void OperationQueue::unregisterNotificationWhenMemoryWarning(void* observer)
{
    WCDB_UNUSED(observer)
}

}

#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

namespace WCDB {

void* OperationQueue::operationStart()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    return (void *) pool;
}

void OperationQueue::operationEnd(void *context)
{
    NSAutoreleasePool *pool = (__bridge NSAutoreleasePool *) context;
    [pool drain];
}

}
