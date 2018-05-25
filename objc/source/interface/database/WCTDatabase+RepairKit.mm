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

@implementation WCTDatabase (RepairKit)

- (BOOL)archiveAsMaterial
{
    return _database->archiveAsMaterial();
}

- (BOOL)backup
{
    return _database->backup();
}

- (BOOL)recoverFromPath:(NSString *)corruptedDBPath
           withPageSize:(const int)pageSize
           backupCipher:(NSData *)backupCipher
         databaseCipher:(NSData *)databaseCipher
{
    return _database->recoverFromPath(corruptedDBPath.cppString,
                                      pageSize,
                                      WCDB::Data::immutableNoCopyData((const unsigned char *) backupCipher.bytes, (size_t) backupCipher.length),
                                      WCDB::Data::immutableNoCopyData((const unsigned char *) databaseCipher.bytes, (size_t) databaseCipher.length));
}

@end
