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

#import <WCDB/WCTCore.h>

@protocol WCTTable

- (BOOL)isTableExists:(nonnull NSString *)tableName;

- (BOOL)isTableExists:(nonnull NSString *)tableName
            withError:(WCTCoreError *_Nullable *_Nullable)error;

- (BOOL)createTableAndIndexes:(nonnull NSString *)tableName
                    withClass:(nonnull Class<WCTTableCoding>)cls;

- (nonnull WCTTable *)getTable:(nonnull NSString *)tableName
                     withClass:(nonnull Class<WCTTableCoding>)cls;

- (BOOL)createVirtualTable:(nonnull NSString *)tableName
                 withClass:(nonnull Class<WCTTableCoding>)cls;

- (BOOL)dropTable:(nonnull NSString *)tableName;

- (BOOL)dropIndex:(nonnull NSString *)indexName;

@end

@interface WCTTable < ObjectType : NSObject <WCTTableCoding>
* > : WCTCore

      /**
 The name of the table.
 */
      @property(nonatomic, readonly) NSString *_Nonnull tableName;

/**
 The class binding to this table.
 */
@property(nonatomic, readonly) Class _Nonnull cls;

- (nullable WCTDatabase *)getDatabase;

@end
