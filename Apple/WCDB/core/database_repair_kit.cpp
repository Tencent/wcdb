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

#include <WCDB/database.hpp>
#include <WCDB/path.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

//const std::string DataBase::s_defaultBackupSubfix = "-srk";
//
//std::string DataBase::defaultBackupPath() const
//{
//    return Path::addExtention(path, s_defaultBackupSubfix);
//}
//
//bool DataBase::backup(const std::string& backupPath, Error& error)
//{
//    RecyclableHandle handle = flowOut(error);
//    bool result = handle->backup(backupPath);
//    error = handle->getError();
//    return result;
//}
//
//bool DataBase::backup(Error& error)
//{
//    return backup(defaultBackupPath(), error);
//}
//
//bool DataBase::repair(const std::string& newDBPath, const Assist& assist, Error& error)
//{
//    return repair(newDBPath, defaultBackupPath(), assist, error);
//}
//
//bool DataBase::repair(const std::string& newDBPath, const std::string& backupPath, const Assist& assist, Error& error)
//{
//    if (!isBlockaded()
//        ||isOpened()) {
//        WCDB::Error::Warning("Repair an opened database may cause a corrupted database");
//    }
//#warning TODO handle repair error
//    return wcsrk_repair(path.c_str(), newDBPath.c_str(), backupPath.c_str(), assist);
//}

}//namespace WCDB
