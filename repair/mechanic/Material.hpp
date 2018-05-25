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

#ifndef Material_hpp
#define Material_hpp

#include <WCDB/ThreadedErrorProne.hpp>
#include <list>
#include <stdlib.h>
#include <string>
#include <vector>

namespace WCDB {

class Data;

namespace Repair {

class Material : protected SharedThreadedErrorProne {
#pragma mark - Initialiazation
public:
    Material();

#pragma mark - Header
public:
    struct Meta {
        uint32_t pageSize;
        uint32_t reservedBytes;

        uint32_t walSalt1;
        uint32_t walSalt2;
        uint32_t walFrame;
    };

    static constexpr const uint32_t magic = 0x57434442;
    static constexpr const uint32_t version = 0x01000000; //1.0.0.0
    static constexpr const int HeaderSize =
        sizeof(uint32_t) * 3 + sizeof(Meta); //magic + version + meta

    Meta meta;

#pragma mark - Content
public:
    class Content {
    public:
        std::string tableName;
        std::string sql;
        std::list<std::string> associatedSQLs;
        std::vector<uint32_t> pagenos;
    };
    std::list<Content> contents;

#pragma mark - Serialization
public:
    bool initWithData(const Data &data);
    Data encodedData() const;

protected:
    void markAsCorrupt();
};

} //namespace Repair

} //namespace WCDB

#endif /* Material_hpp */
