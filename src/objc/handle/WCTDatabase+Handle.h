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

#import "WCTCommon.h"
#import "WCTDatabase.h"

NS_ASSUME_NONNULL_BEGIN

class WCDB_API WCTScopedHandle {
public:
    WCTScopedHandle(WCTHandle *handle = nil);
    ~WCTScopedHandle();
    WCTHandle *getHandle() const;

private:
    WCTHandle *m_handle;
};

WCDB_API @interface WCTDatabase(Handle)
/**
 @brief Create a `WCTHandle` for current database.
 WCTHandle is a wrapper for sqlite db handle of type `sqlite3*`, and the sqlite db handle is lazy initialized and will not be actually generated until the first operation on current handle takes place.
 `WCDB_GET_SCOPED_HANDLE` is a better way to create a WCTHandle, so that the handle will be invalidated automatically when the current scope is finished.
 @note  All WCTHandles created by the current database in the current thread will share the same sqlite db handle internally, so it can avoid the deadlock between different sqlite db handles in some extreme cases.
 @return A WCTHandle object.
 */
- (WCTHandle *)getHandle;

/**
 @brief Execute a statement directly.
 @return YES if no error occurs.
 */
- (BOOL)execute:(const WCDB::Statement &)statement;

/**
 @brief Execute a sql string directly.
 @warning You should no use this method to access or modify the data in a migrating table..
 @see   `-[WCTDatabase addMigration:withFilter:]`
 @return YES if no error occurs.
 */
- (BOOL)rawExecute:(NSString *)sql;

@end

#define WCDB_GET_SCOPED_HANDLE(database, handle) _WCDB_GET_SCOPED_HANDLE(database, handle, __COUNTER__)
#define _WCDB_GET_SCOPED_HANDLE(database, handle, counter) __WCDB_GET_SCOPED_HANDLE(database, handle, counter)
#define __WCDB_GET_SCOPED_HANDLE(database, handle, counter)        \
    WCTScopedHandle __scopedHandle##counter([database getHandle]); \
    WCTHandle *handle = __scopedHandle##counter.getHandle();

NS_ASSUME_NONNULL_END
