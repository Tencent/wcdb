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

#ifndef Cell_hpp
#define Cell_hpp

#include <WCDB/Data.hpp>
#include <WCDB/Serialization.hpp>

namespace WCDB {

namespace Repair {

class Page;

class Cell {
public:
    Cell(int pointer, Page &page);

    bool initialize();

    const Page &getPage() const;

    enum Type {
        Null = 0,
        Integer32 = 1,
        Integer64 = 2,
        Real = 3,
        Text = 4,
        BLOB = 5,
    };
    int64_t getRowID() const;
    Type getValueType(int index) const;
    int getCount() const;

    int32_t int32Value(int index) const;
    int64_t int64Value(int index) const;
    double doubleValue(int index) const;
    std::pair<int, const char *> textValue(int index) const;
    std::string stringValue(int index) const;
    std::pair<int, const unsigned char *> blobValue(int index) const;

protected:
    static int getLengthOfSerialType(int serialType);
    static int isSerialTypeSanity(int serialType);

    Page &m_page;

    int64_t m_rowid;
    int m_pointer;

    Deserialization m_deserialization;
    Data m_payload;
    //serial type -> offset of value
    std::vector<std::pair<int, int>> m_columns;
};

} //namespace Repair

} //namespace WCDB

#endif /* Cell_hpp */
