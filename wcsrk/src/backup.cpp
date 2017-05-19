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

#include <backup.hpp>
#include <sqlite3.h>

const char Backup::MAGIC[4] = {'!', 's', 'k', 'b'};
const int32_t Backup::VERSION = 1;

std::shared_ptr<Backup> Backup::BackupFromHandle(sqlite3* handle)
{
    std::shared_ptr<std::map<std::string, int>> infos(new std::map<std::string, int>);
    int32_t freePageCount = 0;
    if (!infos) {
        return nullptr;
    }
    
    {
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(handle, "SELECT name, rootpage FROM sqlite_master WHERE type='table'", -1, &stmt, nullptr);
        if (rc!=SQLITE_OK) {
            return nullptr;
        }
        
        while (1) {
            rc = sqlite3_step(stmt);
            if (rc!=SQLITE_ROW) {
                break;
            }
            const char* name = (const char*)sqlite3_column_text(stmt, 0);
            int rootpage = sqlite3_column_int(stmt, 1);
            infos->insert({name, rootpage});
        }
        sqlite3_finalize(stmt);
        if (rc!=SQLITE_DONE) {
            return nullptr;
        }
    }
    
    {
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare(handle, "PRAGMA freelist_count", -1, &stmt, nullptr);
        if (rc!=SQLITE_OK) {
            return nullptr;
        }
        rc = sqlite3_step(stmt);
        if (rc==SQLITE_ROW) {
            freePageCount = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    
    return std::shared_ptr<Backup>(new Backup(infos, freePageCount));
}

std::shared_ptr<Backup> Backup::BackupFromDataBase(const std::string& dbPath)
{
    sqlite3* handle;
    int rc = sqlite3_open(dbPath.c_str(), &handle);
    if (rc!=SQLITE_OK) {
        return nullptr;
    }
    std::shared_ptr<Backup> backup = BackupFromHandle(handle);
    sqlite3_close(handle);
    return backup;
}

std::shared_ptr<Backup> Backup::DecodeFromPath(const std::string& backupPath)
{
#warning TODO 反序列化
    return nullptr;
}

Backup::Backup(const std::shared_ptr<std::map<std::string, int>>& i
               , int32_t f)
: infos(i)
, freePageCount(f)
{
}

std::shared_ptr<unsigned char> Backup::encode(size_t& size)
{
#warning TODO 序列化
//    Backup::MAGIC
//    Backup::VERSION
    return nullptr;
}

