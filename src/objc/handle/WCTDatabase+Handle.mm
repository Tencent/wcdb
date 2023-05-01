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

#import "WCTDatabase+Handle.h"
#import "WCTDatabase+Private.h"
#import "WCTHandle+Private.h"

WCTScopedHandle::WCTScopedHandle(WCTHandle *handle)
: m_handle(handle)
{
}

WCTScopedHandle::~WCTScopedHandle()
{
    if (m_handle != nil) {
        [m_handle finalizeStatement];
        [m_handle invalidate];
    }
}

WCTHandle *WCTScopedHandle::getHandle() const
{
    return m_handle;
}

@implementation WCTDatabase (Handle)

- (WCTHandle *)getHandle
{
    return [[WCTHandle alloc] initWithDatabase:self];
}

- (BOOL)execute:(const WCDB::Statement &)statement
{
    return _database->execute(statement);
}

//rawExecute should no be used to access or modify the data in a migrating table.
- (BOOL)rawExecute:(NSString *)sql
{
    return _database->execute(sql);
}

@end
