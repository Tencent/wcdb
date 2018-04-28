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

@implementation WCTMigrationInfo

- (instancetype)init
{
    WCTFatalError("Use [initWithTargetTable:fromSourceTable:ofDatabase:] instead.");
    return [self initWithWCDBMigrationInfo:nullptr];
}

- (instancetype)initWithTargetTable:(NSString *)targetTable
                    fromSourceTable:(NSString *)sourceTable
                         ofDatabase:(NSString *)sourceDatabasePath
{
    WCTRemedialAssert(targetTable && sourceTable, "Table name can't be null.", return nil;);
    if (self = [super init]) {
        _targetTable = targetTable;
        _sourceTable = sourceTable;
        _sourceDatabasePath = sourceDatabasePath;
    }
    return self;
}

- (instancetype)initWithTargetTable:(NSString *)targetTable
                    fromSourceTable:(NSString *)sourceTable
{
    return [self initWithTargetTable:targetTable
                     fromSourceTable:sourceTable
                          ofDatabase:nil];
}

- (instancetype)initWithWCDBMigrationInfo:(const WCDB::MigrationInfo *)info
{
    if (info == nullptr) {
        return nil;
    }
    NSString *targetTable = [NSString stringWithCppString:info->targetTable];
    NSString *sourceTable = [NSString stringWithCppString:info->targetTable];
    NSString *sourceDatabasePath = info->sourceDatabasePath.empty() ? nil : [NSString stringWithCppString:info->sourceDatabasePath];
    return [self initWithTargetTable:targetTable
                     fromSourceTable:sourceTable
                          ofDatabase:sourceDatabasePath];
}

- (std::shared_ptr<WCDB::MigrationInfo>)getWCDBMigrationInfo
{
    return WCDB::MigrationInfo::info(_targetTable.cppString,
                                     _sourceTable.cppString,
                                     _sourceDatabasePath ? _sourceDatabasePath.cppString : WCDB::String::empty());
}

- (NSString *)schema;
{
    if (_sourceDatabasePath) {
        return [NSString stringWithCppString:WCDB::MigrationInfo::resolvedSchema(_sourceDatabasePath.cppString)];
    }
    return nil;
}

@end
