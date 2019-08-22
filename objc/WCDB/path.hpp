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

#ifndef path_hpp
#define path_hpp

#include <string>

namespace WCDB {

namespace Path {
std::string addExtention(const std::string &base, const std::string &extention);
std::string addComponent(const std::string &base, const std::string &component);
std::string getFileName(const std::string &base);
std::string getBaseName(const std::string &base);

#ifdef _WIN32

#define STRING

#ifndef STRING
#define WSTRING
#endif // !STRING

#ifdef STRING
using string_type = std::string;
using char_type = char;
#elif defined(WSTRING)
using string_type = std::wstring;
using char_type = wchar_t;
#endif

bool is_separator(char_type c);
string_type::size_type root_directory_start(const string_type &path,
                                            string_type::size_type size);
string_type::size_type filename_pos(const string_type &str,
                                    string_type::size_type end_pos);
string_type::size_type m_parent_path_end(string_type path);
string_type parent_path(string_type path);

#endif 
} //namespace Path

} //namespace WCDB

#endif /* path_hpp */
