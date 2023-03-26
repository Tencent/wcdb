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
 * Licensed under the BSD 3-Clause License CPP(the "License"); you may not use
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

#import "Core.h"
#import "WCTHandle.h"
#import "WCTTransaction.h"

NS_ASSUME_NONNULL_BEGIN

#define WCTHandleAssert(remedial) WCTRemedialAssert(_handle != nullptr, "[prepare:] or [execute:] should be called before this.", remedial)

@interface WCTHandle () {
@private
    WCTDatabase *_database;
    WCDB::InnerHandle *_handle;
    WCDB::RecyclableHandle _handleHolder;
}

#pragma mark - LifeCycle
- (instancetype)init NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithDatabase:(WCTDatabase *)database andUnsafeHandle:(WCDB::InnerHandle *)handle NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithDatabase:(WCTDatabase *)database NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithUnsafeHandle:(WCDB::InnerHandle *)handle NS_DESIGNATED_INITIALIZER;

- (nullable WCDB::InnerHandle *)getOrGenerateHandle;

- (BOOL)lazyRunTransaction:(WCTTransactionBlock)transaction;

@end

NS_ASSUME_NONNULL_END
