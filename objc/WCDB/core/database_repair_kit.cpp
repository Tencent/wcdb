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

#ifndef COCOAPODS
#include <WCDB/SQLiteRepairKit.h>
#else
#include <sqliterk/SQLiteRepairKit.h>
#endif
#include <WCDB/database.hpp>
#include <WCDB/path.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

bool Database::backup(const void *key, const unsigned int &length, Error &error)
{
    RecyclableHandle handle = flowOut(error);
    if (!handle) {
        return false;
    }
    bool result = handle->backup(key, length);
    error = handle->getError();
    return result;
}

bool Database::recoverFromPath(const std::string &corruptedDBPath,
                               const int pageSize,
                               const void *backupKey,
                               const unsigned int &backupKeyLength,
                               const void *databaseKey,
                               const unsigned int &databaseKeyLength,
                               Error &error)
{
    RecyclableHandle handle = flowOut(error);
    if (!handle) {
        return false;
    }
    bool result = handle->recoverFromPath(corruptedDBPath, pageSize, backupKey,
                                          backupKeyLength, databaseKey,
                                          databaseKeyLength);
    error = handle->getError();
    return result;
}

} //namespace WCDB
