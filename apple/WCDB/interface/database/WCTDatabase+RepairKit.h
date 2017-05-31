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
#import <WCDB/WCTDatabase.h>

@interface WCTDatabase (RepairKit)

/**
 @brief Backup metadata to recover. Since metadata will be changed while a table or an index is created or dropped, you should call this periodically.

 @param key The cipher key for backup
 @return YES only if it's successfully backed up
 */
- (BOOL)backupWithCipher:(NSData *)key;

/**
 @brief Recover data from a corruped db. You'd better to recover a closed database. A new repair kit is developing at https://github.com/Tencent/wcdb/tree/new-repair
 
     WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
     WCTDatabase* corruped = [[WCTDatabase alloc] initWithPath:corrupedDBPath];
     [corruped close:^(){
         BOOL result = [database recoverFromPath:corrupedDBPath withPageSize:4096 backupCipher:backupCipher databaseCipher:databaseCipher];
         NSLog(@"result %d", result);
     }];

 @param corruptedDBPath The path to the corrupted database
 @param pageSize Page size of the corrupted database. It's default to 4096 on iOS. Page size never change unless you can call "PRAGMA page_size=NewPageSize" to set it. Also, you can call "PRAGMA page_size" to check the current value while database is not corrupted.
 @param backupCipher The cipher key for backup
 @param databaseCipher The cipher key for corrupeted database
 @return YES only if it's successfully recovered.
 */
- (BOOL)recoverFromPath:(NSString *)corruptedDBPath withPageSize:(const int)pageSize backupCipher:(NSData *)backupCipher databaseCipher:(NSData *)databaseCipher;

@end
