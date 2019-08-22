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

#ifndef file_hpp
#define file_hpp

#include <WCDB/error.hpp>
#include <list>
#include <string>

namespace WCDB {

namespace File {
//Basic
bool isExists(const std::string &path, Error &error);
size_t getFileSize(const std::string &path, Error &error);
bool createHardLink(const std::string &from,
                    const std::string &to,
                    Error &error);
bool removeHardLink(const std::string &path, Error &error);
bool removeFile(const std::string &path, Error &error);
bool createDirectory(const std::string &path, Error &error);
//Combination
size_t getFilesSize(const std::list<std::string> &paths, Error &error);
bool removeFiles(const std::list<std::string> &paths, Error &error);
bool moveFiles(const std::list<std::string> &paths,
               const std::string &directory,
               Error &error);
bool createDirectoryWithIntermediateDirectories(const std::string &path,
                                                Error &error);
} //namespace File

} //namespace WCDB

#endif /* file_hpp */
