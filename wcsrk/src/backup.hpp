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

#ifndef backup_hpp
#define backup_hpp

#include <stdio.h>
#include <map>
#include <string>

typedef struct sqlite3 sqlite3;

class Backup
{
public:
    static const char MAGIC[4];
    static const int32_t VERSION;

    static std::shared_ptr<Backup> BackupFromHandle(sqlite3* handle);
    static std::shared_ptr<Backup> BackupFromDatabase(const std::string& dbPath);
    static std::shared_ptr<Backup> DecodeFromPath(const std::string& backupPath);
    
    std::shared_ptr<unsigned char> encode(size_t& size);
    
    const std::shared_ptr<std::map<std::string, int32_t>> infos;//table name -> root page
    const int32_t freePageCount;
protected:
    Backup(const std::shared_ptr<std::map<std::string, int>>& infos
           , int32_t freePageCount);
};

#endif /* backup_hpp */
