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

#ifndef KeyValueTable_hpp
#define KeyValueTable_hpp

#include <WCDB/Abstract.h>

namespace WCDB {

class KeyValueTable {
public:
    KeyValueTable(Handle *handle);

    KeyValueTable() = delete;
    KeyValueTable(const KeyValueTable &) = delete;
    KeyValueTable &operator=(const KeyValueTable &) = delete;

    std::pair<bool, bool> isTableExists();
    bool createTable();

    std::pair<bool, std::string> getMigratingValue();
    bool setMigratingValue(const std::string &value);

    static const Column getKey();
    static const Column getValue();
    static const std::string getTable();

protected:
    std::pair<bool, std::string> getTextValue(const std::string &key);
    bool setTextValue(const std::string &key, const std::string &value);

    Handle *m_handle;
};

} //namespace WCDB

#endif /* KeyValueTable_hpp */
