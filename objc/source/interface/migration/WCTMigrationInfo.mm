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
#import <WCDB/NSString+CppString.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTMigrationInfo+Private.h>

@implementation WCTMigrationInfo {
    std::shared_ptr<WCDB::MigrationInfo> _info;
}

- (instancetype)initWithTargetTable:(NSString *)targetTable
                    fromSourceTable:(NSString *)sourceTable
                         ofDatabase:(NSString *)sourceDatabasePath
{
    WCTRemedialAssert(targetTable && sourceTable, "Table name can't be null.", return nil;);
    std::shared_ptr<WCDB::MigrationInfo> info = WCDB::MigrationInfo::info(targetTable.cppString, sourceTable.cppString, sourceDatabasePath ? sourceDatabasePath.cppString : WCDB::String::empty());
    return [self initWithWCDBMigrationInfo:info];
}

- (instancetype)initWithTargetTable:(NSString *)targetTable
                    fromSourceTable:(NSString *)sourceTable
{
    return [self initWithTargetTable:targetTable fromSourceTable:sourceTable ofDatabase:nil];
}

- (instancetype)initWithWCDBMigrationInfo:(const std::shared_ptr<WCDB::MigrationInfo> &)info
{
    if ((self = [super init]) && info != nullptr) {
        _info = info;
        return self;
    }
    return nil;
}

- (const std::shared_ptr<WCDB::MigrationInfo> &)getWCDBMigrationInfo
{
    return _info;
}

- (nonnull NSString *)targetTable
{
    return [NSString stringWithCppString:_info->targetTable];
}

- (nonnull NSString *)sourceTable
{
    return [NSString stringWithCppString:_info->sourceTable];
}

- (nullable NSString *)sourceDatabasePath
{
    const std::string &path = _info->sourceDatabasePath;
    if (path.empty()) {
        return nil;
    }
    return [NSString stringWithCppString:path];
}

@end
