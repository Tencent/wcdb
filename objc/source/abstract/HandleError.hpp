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

#ifndef HandleError_hpp
#define HandleError_hpp

#include <WCDB/Error.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class HandleError : public Error {
public:
    typedef int64_t Tag;

    enum Operation {
        NotSet = 0,
        Prepare = 1,
        Execute = 2,
        Open = 3,
        Step = 4,
        Cipher = 5,
        Repair = 6,
        Backup = 7,
    };

    Tag tag;
    int extendedCode;
    std::string path;
    Statement statement;
    Operation operation;

    virtual std::string getDescription() const override;
    virtual size_t getHashedTypeid() const override;

    void reset();
};

} //namespace WCDB

#endif /* HandleError_hpp */
