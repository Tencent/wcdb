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

#ifndef RepairError_hpp
#define RepairError_hpp

#include <WCDB/Error.hpp>

namespace WCDB {

namespace Repair {

class RepairError : public Error {
public:
    RepairError();

    enum Code {
        OK = 0,
        Corrupt = 1,
        NoMemory = 2,
        CantOpen = 3,
        IOError = 4,
        Full = 5,
        Unknown = 6,
    };

    enum ExtendedType {
        NotSet,
        SQLite,
        System,
    };
    ExtendedType extendedType;
    int extendedCode;

    void setupCode(Code code);
    void setupSQLiteCode(int code, int extendedCode_);
    void setupSystemErrno(int code);

    std::string getDescription() const override;
};

} //namespace Repair

} //namespace WCDB

#endif /* RepairError_hpp */
