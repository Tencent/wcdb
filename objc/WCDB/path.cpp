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

#include <WCDB/path.hpp>
#ifndef _WIN32
#include <libgen.h>
#endif

namespace WCDB {

namespace Path {

std::string addExtention(const std::string &base, const std::string &extention)
{
    return base + extention;
}

std::string addComponent(const std::string &base, const std::string &component)
{
    std::string newPath = base;
    if (newPath[newPath.size() - 1] != '/') {
        newPath.append("/");
    }
    newPath.append(component);
    return newPath;
}

std::string getFileName(const std::string &base)
{
    size_t size = base.size();
    bool findLast = true;
    const char *cstr = base.c_str();
    size_t begin = 0, end = 0, length = 0;
    for (size_t i = 0; i < size; ++i) {
        size_t pos = size - i - 1;
        if (findLast) {
            if (cstr[pos] != '/') {
                end = pos;
                findLast = false;
            }
        } else {
            if (cstr[pos] == '/') {
                length = end - pos;
                begin = pos + 1;
                break;
            }
        }
    }
    return length != 0 && size > 0 ? std::string(cstr, begin, length) : "/";
}

std::string getBaseName(const std::string &base){
    std::string dir = base;
#ifdef _WIN32
    dir = parent_path(dir);
#else
	auto tmp = (&(*dir.begin()));
    dir = dirname(&(*dir.begin()));
#endif
    return dir;
}

#ifdef _WIN32

#ifdef STRING
const char separator = '/';
const char *const separators = "/\\";
const char preferred_separator = '\\';
const char colon = ':';
const char questionmark = '?';
#elif defined(WSTRING)
const wchar_t separator = L'/';
const wchar_t *const separators = L"/\\";
const wchar_t preferred_separator = L'\\';
const wchar_t colon = L':';
const wchar_t questionmark = L'?';
#endif

bool is_separator(char_type c)
{
    return c == separator || c == preferred_separator;
}

// return npos if no root_directory found
string_type::size_type root_directory_start(const string_type &path,
                                            string_type::size_type size)
{
    // case "c:/"
    if (size > 2 && path[1] == colon && is_separator(path[2]))
        return 2;

    // case "//"
    if (size == 2 && is_separator(path[0]) && is_separator(path[1]))
        return string_type::npos;

    // case "\\?\"
    if (size > 4 && is_separator(path[0]) && is_separator(path[1]) &&
        path[2] == questionmark && is_separator(path[3])) {
        string_type::size_type pos(path.find_first_of(separators, 4));
        return pos < size ? pos : string_type::npos;
    }

    // case "//net {/}"
    if (size > 3 && is_separator(path[0]) && is_separator(path[1]) &&
        !is_separator(path[2])) {
        string_type::size_type pos(path.find_first_of(separators, 2));
        return pos < size ? pos : string_type::npos;
    }

    // case "/"
    if (size > 0 && is_separator(path[0]))
        return 0;

    return string_type::npos;
}

// end_pos is past-the-end position
// return 0 if str itself is filename (or empty)
string_type::size_type filename_pos(const string_type &str,
                                    string_type::size_type end_pos)
{
    // case: "//"
    if (end_pos == 2 && is_separator(str[0]) && is_separator(str[1]))
        return 0;

    // case: ends in "/"
    if (end_pos && is_separator(str[end_pos - 1]))
        return end_pos - 1;

    // set pos to start of last element
    string_type::size_type pos(str.find_last_of(separators, end_pos - 1));

    if (pos == string_type::npos && end_pos > 1)
        pos = str.find_last_of(colon, end_pos - 2);

    return (pos ==
                string_type::npos // path itself must be a filename (or empty)
            || (pos == 1 && is_separator(str[0]))) // or net
               ? 0        // so filename is entire string
               : pos + 1; // or starts after delimiter
}

string_type::size_type m_parent_path_end(string_type path)
{
    string_type::size_type end_pos(filename_pos(path, path.size()));

    bool filename_was_separator(path.size() && is_separator(path[end_pos]));

    // skip separators unless root directory
    string_type::size_type root_dir_pos(root_directory_start(path, end_pos));
    for (; end_pos > 0 && (end_pos - 1) != root_dir_pos &&
           is_separator(path[end_pos - 1]);
         --end_pos) {
    }

    return (end_pos == 1 && root_dir_pos == 0 && filename_was_separator)
               ? string_type::npos
               : end_pos;
}

string_type parent_path(string_type path)
{
    string_type::size_type end_pos(m_parent_path_end(path));
    if (end_pos == string_type::npos) {
#ifdef STRING
        return "";
#elif defined(WSTRING)
        return L"";
#endif
    } else
        return path.substr(0, end_pos);
}

#endif //_WIN32

} //namespace Path

} //namespace WCDB
