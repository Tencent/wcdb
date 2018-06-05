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

- (BOOL)deposit
{
    return _database->deposit();
}

- (BOOL)backup
{
    return _database->backup(nullptr);
}

- (BOOL)backup:(WCTBackupFilterBlock)shouldTableBeBackedUp
{
    WCDB::Database::BackupFilter filter = nullptr;
    if (shouldTableBeBackedUp) {
        filter = [shouldTableBeBackedUp](const std::string &tableName) -> bool {
            return shouldTableBeBackedUp([NSString stringWithCppString:tableName]);
        };
    }
    return _database->backup(filter);
}

- (BOOL)retrieve
{
    return _database->retrieve();
}

@end
