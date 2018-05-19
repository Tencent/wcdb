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

#include <WCDB/Assertion.hpp>
#include <WCDB/RepairError.hpp>
#include <errno.h>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

namespace Repair {

RepairError::RepairError()
    : Error(), extendedCode(0), extendedType(ExtendedType::NotSet)
{
}

void RepairError::setupSQLiteCode(int code_, int extendedCode_)
{
    switch (code_) {
        case SQLITE_CORRUPT:
        case SQLITE_NOTADB:
            code = Code::Corrupt;
            break;
        case SQLITE_IOERR:
            code = Code::IOError;
            break;
        case SQLITE_CANTOPEN:
            code = Code::CantOpen;
            break;
        case SQLITE_FULL:
            code = Code::Full;
            break;
        case SQLITE_NOMEM:
            code = Code::NoMemory;
            break;
        default:
            code = Code::Unknown;
            break;
    }
    extendedType = ExtendedType::SQLite;
    extendedCode = extendedCode_;
}

void RepairError::setupSystemErrno(int code_)
{
    switch (code_) {
        case EACCES:
            code = Code::CantOpen;
            break;
        case ENOMEM:
            code = Code::NoMemory;
            break;
        case ENOSPC:
            code = Code::Full;
            break;
        case EIO:
            code = Code::IOError;
            break;
        default:
            code = Code::Unknown;
            break;
    }
    extendedType = ExtendedType::System;
    extendedCode = code_;
}

void RepairError::setupCode(Code code_)
{
    WCTInnerAssert(code_ == Code::NoMemory || code_ == Code::Corrupt) code =
        code_;
    extendedType = ExtendedType::NotSet;
    extendedCode = 0;
}

std::string RepairError::getDescription() const
{
    std::string description = Error::getDescription();
    if (extendedType != ExtendedType::NotSet) {
        addToDescription(description, "ExtendedType", extendedType);
        addToDescription(description, "ExtendedCode", extendedCode);
    }
    return description;
}

} //namespace Repair

} //namespace WCDB
