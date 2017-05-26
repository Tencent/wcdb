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

#import <WCDB/WCTDatabase+RepairKit.h>
#import <WCDB/WCTDatabase+Private.h>

@implementation WCTDatabase(RepairKit)

//- (NSString*)defaultBackupPath
//{
//    return @(_database->defaultBackupPath().c_str());
//}
//
//- (BOOL)backup
//{
//    WCDB::Error wcdbError;
//    return _database->backup(wcdbError);
//}
//
//- (BOOL)backupToPath:(NSString*)backupPath
//{
//    WCDB::Error wcdbError;
//    return _database->backup(backupPath.UTF8String, wcdbError);
//}
//
//- (BOOL)removeBackup
//{
//    WCDB::Error wcdbError;
//    return _database->removeBackup(wcdbError);
//}
//
//- (BOOL)removeBackupAtPath:(NSString*)backupPath
//{
//    WCDB::Error wcdbError;
//    return _database->removeBackup(backupPath.UTF8String, wcdbError);
//}

//- (BOOL)repairToPath:(NSString*)newDBPath withBackupPath:(NSString*)backupPath andAssist:(const Assist&)assist
//{
//    WCDB::Error wcdbError;
//    return _database->repair(newDBPath.UTF8String, backupPath.UTF8String, assist, wcdbError);
//}
//
//- (BOOL)repairToPath:(NSString*)newDBPath andAssist:(const Assist&)assist
//{
//    WCDB::Error wcdbError;
//    return _database->repair(newDBPath.UTF8String, assist, wcdbError);
//}
@end
