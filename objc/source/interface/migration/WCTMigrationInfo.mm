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
#import <WCDB/WCTMigrationInfo+Private.h>

@implementation WCTMigrationInfo {
    std::shared_ptr<WCDB::MigrationInfo> _info;
}

- (instancetype)initWithTargetTable:(NSString *)targetTable
                    fromSourceTable:(NSString *)sourceTable
                         ofDatabase:(NSString *)sourceDatabasePath
{
    std::shared_ptr<WCDB::MigrationInfo> info = WCDB::MigrationInfo::info(targetTable.UTF8String,
                                                                          sourceTable.UTF8String,
                                                                          sourceDatabasePath.UTF8String);
    return [self initWithWCDBMigrationInfo:info];
}

- (instancetype)initWithTargetTable:(NSString *)targetTable
                    fromSourceTable:(NSString *)sourceTable
{
    std::shared_ptr<WCDB::MigrationInfo> info =
        WCDB::MigrationInfo::info(targetTable.UTF8String,
                                  sourceTable.UTF8String,
                                  "");
    return [self initWithWCDBMigrationInfo:info];
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
    return @(_info->targetTable.c_str());
}

- (nonnull NSString *)sourceTable
{
    return @(_info->sourceTable.c_str());
}

- (nullable NSString *)sourceDatabasePath
{
    const std::string &path = _info->sourceDatabasePath;
    if (path.empty()) {
        return nil;
    }
    return @(path.c_str());
}

@end
