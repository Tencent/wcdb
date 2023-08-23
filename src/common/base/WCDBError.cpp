//
// Created by sanhuazhang on 2018/05/19
//

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

#include "WCDBError.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "CrossPlatform.h"
#include "SQLite.h"
#include "StringView.hpp"
#include <cstring>
#include <sstream>
#include <system_error>
#ifdef _WIN32
#include <windows.h>
#endif

namespace WCDB {

// Code
static_assert((int) Error::Code::OK == SQLITE_OK, "");
static_assert((int) Error::Code::Error == SQLITE_ERROR, "");
static_assert((int) Error::Code::Internal == SQLITE_INTERNAL, "");
static_assert((int) Error::Code::Permission == SQLITE_PERM, "");
static_assert((int) Error::Code::Abort == SQLITE_ABORT, "");
static_assert((int) Error::Code::Busy == SQLITE_BUSY, "");
static_assert((int) Error::Code::Locked == SQLITE_LOCKED, "");
static_assert((int) Error::Code::NoMemory == SQLITE_NOMEM, "");
static_assert((int) Error::Code::Readonly == SQLITE_READONLY, "");
static_assert((int) Error::Code::Interrupt == SQLITE_INTERRUPT, "");
static_assert((int) Error::Code::IOError == SQLITE_IOERR, "");
static_assert((int) Error::Code::Corrupt == SQLITE_CORRUPT, "");
static_assert((int) Error::Code::NotFound == SQLITE_NOTFOUND, "");
static_assert((int) Error::Code::Full == SQLITE_FULL, "");
static_assert((int) Error::Code::CantOpen == SQLITE_CANTOPEN, "");
static_assert((int) Error::Code::Protocol == SQLITE_PROTOCOL, "");
static_assert((int) Error::Code::Empty == SQLITE_EMPTY, "");
static_assert((int) Error::Code::Schema == SQLITE_SCHEMA, "");
static_assert((int) Error::Code::Exceed == SQLITE_TOOBIG, "");
static_assert((int) Error::Code::Constraint == SQLITE_CONSTRAINT, "");
static_assert((int) Error::Code::Mismatch == SQLITE_MISMATCH, "");
static_assert((int) Error::Code::Misuse == SQLITE_MISUSE, "");
static_assert((int) Error::Code::NoLargeFileSupport == SQLITE_NOLFS, "");
static_assert((int) Error::Code::Authorization == SQLITE_AUTH, "");
static_assert((int) Error::Code::Format == SQLITE_FORMAT, "");
static_assert((int) Error::Code::Range == SQLITE_RANGE, "");
static_assert((int) Error::Code::NotADatabase == SQLITE_NOTADB, "");
static_assert((int) Error::Code::Notice == SQLITE_NOTICE, "");
static_assert((int) Error::Code::Warning == SQLITE_WARNING, "");
static_assert((int) Error::Code::Row == SQLITE_ROW, "");
static_assert((int) Error::Code::Done == SQLITE_DONE, "");

// ExtCode
static_assert((int) Error::ExtCode::ErrorMissingCollseq == SQLITE_ERROR_MISSING_COLLSEQ, "");
static_assert((int) Error::ExtCode::ErrorRetry == SQLITE_ERROR_RETRY, "");
static_assert((int) Error::ExtCode::ErrorSnapshot == SQLITE_ERROR_SNAPSHOT, "");
static_assert((int) Error::ExtCode::IOErrorRead == SQLITE_IOERR_READ, "");
static_assert((int) Error::ExtCode::IOErrorShortRead == SQLITE_IOERR_SHORT_READ, "");
static_assert((int) Error::ExtCode::IOErrorWrite == SQLITE_IOERR_WRITE, "");
static_assert((int) Error::ExtCode::IOErrorFsync == SQLITE_IOERR_FSYNC, "");
static_assert((int) Error::ExtCode::IOErrorDirFsync == SQLITE_IOERR_DIR_FSYNC, "");
static_assert((int) Error::ExtCode::IOErrorTruncate == SQLITE_IOERR_TRUNCATE, "");
static_assert((int) Error::ExtCode::IOErrorFstat == SQLITE_IOERR_FSTAT, "");
static_assert((int) Error::ExtCode::IOErrorUnlock == SQLITE_IOERR_UNLOCK, "");
static_assert((int) Error::ExtCode::IOErrorRdlock == SQLITE_IOERR_RDLOCK, "");
static_assert((int) Error::ExtCode::IOErrorDelete == SQLITE_IOERR_DELETE, "");
static_assert((int) Error::ExtCode::IOErrorBlocked == SQLITE_IOERR_BLOCKED, "");
static_assert((int) Error::ExtCode::IOErrorNoMemory == SQLITE_IOERR_NOMEM, "");
static_assert((int) Error::ExtCode::IOErrorAccess == SQLITE_IOERR_ACCESS, "");
static_assert((int) Error::ExtCode::IOErrorCheckReservedLock == SQLITE_IOERR_CHECKRESERVEDLOCK,
              "");
static_assert((int) Error::ExtCode::IOErrorLock == SQLITE_IOERR_LOCK, "");
static_assert((int) Error::ExtCode::IOErrorClose == SQLITE_IOERR_CLOSE, "");
static_assert((int) Error::ExtCode::IOErrorDirClose == SQLITE_IOERR_DIR_CLOSE, "");
static_assert((int) Error::ExtCode::IOErrorShmOpen == SQLITE_IOERR_SHMOPEN, "");
static_assert((int) Error::ExtCode::IOErrorShmSize == SQLITE_IOERR_SHMSIZE, "");
static_assert((int) Error::ExtCode::IOErrorShmLock == SQLITE_IOERR_SHMLOCK, "");
static_assert((int) Error::ExtCode::IOErrorShmMap == SQLITE_IOERR_SHMMAP, "");
static_assert((int) Error::ExtCode::IOErrorSeek == SQLITE_IOERR_SEEK, "");
static_assert((int) Error::ExtCode::IOErrorDeleteNoEntry == SQLITE_IOERR_DELETE_NOENT, "");
static_assert((int) Error::ExtCode::IOErrorMmap == SQLITE_IOERR_MMAP, "");
static_assert((int) Error::ExtCode::IOErrorGetTempPath == SQLITE_IOERR_GETTEMPPATH, "");
static_assert((int) Error::ExtCode::IOErrorConvPath == SQLITE_IOERR_CONVPATH, "");
static_assert((int) Error::ExtCode::IOErrorVnode == SQLITE_IOERR_VNODE, "");
static_assert((int) Error::ExtCode::IOErrorAuthorization == SQLITE_IOERR_AUTH, "");
static_assert((int) Error::ExtCode::IOErrorBeginAtomic == SQLITE_IOERR_BEGIN_ATOMIC, "");
static_assert((int) Error::ExtCode::IOErrorCommitAtomic == SQLITE_IOERR_COMMIT_ATOMIC, "");
static_assert((int) Error::ExtCode::IOErrorRollbackAtomic == SQLITE_IOERR_ROLLBACK_ATOMIC, "");
static_assert((int) Error::ExtCode::LockedSharedCache == SQLITE_LOCKED_SHAREDCACHE, "");
static_assert((int) Error::ExtCode::LockedVirtualTable == SQLITE_LOCKED_VTAB, "");
static_assert((int) Error::ExtCode::BusyRecovery == SQLITE_BUSY_RECOVERY, "");
static_assert((int) Error::ExtCode::BusySnapshot == SQLITE_BUSY_SNAPSHOT, "");
static_assert((int) Error::ExtCode::CantOpenNoTempDir == SQLITE_CANTOPEN_NOTEMPDIR, "");
static_assert((int) Error::ExtCode::CantOpenIsDir == SQLITE_CANTOPEN_ISDIR, "");
static_assert((int) Error::ExtCode::CantOpenFullPath == SQLITE_CANTOPEN_FULLPATH, "");
static_assert((int) Error::ExtCode::CantOpenConvPath == SQLITE_CANTOPEN_CONVPATH, "");
static_assert((int) Error::ExtCode::CantOpenDirtyWal == SQLITE_CANTOPEN_DIRTYWAL, "");
static_assert((int) Error::ExtCode::CorruptVirtualTable == SQLITE_CORRUPT_VTAB, "");
static_assert((int) Error::ExtCode::CorruptSequence == SQLITE_CORRUPT_SEQUENCE, "");
static_assert((int) Error::ExtCode::ReadonlyRecovery == SQLITE_READONLY_RECOVERY, "");
static_assert((int) Error::ExtCode::ReadonlyCantLock == SQLITE_READONLY_CANTLOCK, "");
static_assert((int) Error::ExtCode::ReadonlyRollback == SQLITE_READONLY_ROLLBACK, "");
static_assert((int) Error::ExtCode::ReadonlyDatabaseMoved == SQLITE_READONLY_DBMOVED, "");
static_assert((int) Error::ExtCode::ReadonlyCantInit == SQLITE_READONLY_CANTINIT, "");
static_assert((int) Error::ExtCode::ReadonlyDirectory == SQLITE_READONLY_DIRECTORY, "");
static_assert((int) Error::ExtCode::AbortRollback == SQLITE_ABORT_ROLLBACK, "");
static_assert((int) Error::ExtCode::ConstraintCheck == SQLITE_CONSTRAINT_CHECK, "");
static_assert((int) Error::ExtCode::ConstraintCommitHook == SQLITE_CONSTRAINT_COMMITHOOK, "");
static_assert((int) Error::ExtCode::ConstraintForeignKey == SQLITE_CONSTRAINT_FOREIGNKEY, "");
static_assert((int) Error::ExtCode::ConstraintFunction == SQLITE_CONSTRAINT_FUNCTION, "");
static_assert((int) Error::ExtCode::ConstraintNotNull == SQLITE_CONSTRAINT_NOTNULL, "");
static_assert((int) Error::ExtCode::ConstraintPrimaryKey == SQLITE_CONSTRAINT_PRIMARYKEY, "");
static_assert((int) Error::ExtCode::ConstraintTrigger == SQLITE_CONSTRAINT_TRIGGER, "");
static_assert((int) Error::ExtCode::ConstraintUnique == SQLITE_CONSTRAINT_UNIQUE, "");
static_assert((int) Error::ExtCode::ConstraintVirtualTable == SQLITE_CONSTRAINT_VTAB, "");
static_assert((int) Error::ExtCode::ConstraintRowID == SQLITE_CONSTRAINT_ROWID, "");
static_assert((int) Error::ExtCode::NoticeRecoverWal == SQLITE_NOTICE_RECOVER_WAL, "");
static_assert((int) Error::ExtCode::NoticeRecoverRollback == SQLITE_NOTICE_RECOVER_ROLLBACK, "");
static_assert((int) Error::ExtCode::WarningAutoIndex == SQLITE_WARNING_AUTOINDEX, "");
static_assert((int) Error::ExtCode::AuthorizationUser == SQLITE_AUTH_USER, "");
static_assert((int) Error::ExtCode::OKLoadPermanently == SQLITE_OK_LOAD_PERMANENTLY, "");

#pragma mark - Initialize
Error::Error() : level(Level::Ignore), m_code(Code::OK)
{
}

Error::Error(Code code, Level level_, const UnsafeStringView& message)
: level(level_)
{
    setCode(code, message);
}

Error::Error(Error&& other)
: level(other.level)
, m_code(other.m_code)
, m_message(std::move(other.m_message))
, infos(std::move(other.infos))
{
    other.level = Level::Ignore;
    other.m_code = Code::OK;
    other.m_message.clear();
}

Error::Error(const Error& other)
: level(other.level)
, m_code(other.m_code)
, m_message(std::move(other.m_message))
, infos(std::move(other.infos))
{
}

Error& Error::operator=(Error&& other)
{
    level = other.level;
    m_code = other.m_code;
    m_message = std::move(other.m_message);
    infos = std::move(other.infos);
    other.level = Level::Ignore;
    other.m_code = Code::OK;
    other.m_message.clear();
    return *this;
}

Error& Error::operator=(const Error& other)
{
    level = other.level;
    m_code = other.m_code;
    m_message = std::move(other.m_message);
    infos = std::move(other.infos);
    return *this;
}

#pragma mark - Level
bool Error::isIgnorable() const
{
    return level == Level::Ignore;
}

#pragma mark - Code
bool Error::isError(int rc)
{
    return rc != SQLITE_OK && rc != SQLITE_ROW && rc != SQLITE_DONE;
}

Error::Code Error::rc2c(int rc)
{
    return (Error::Code)(rc & 0xff);
}

int Error::c2rc(Error::Code code)
{
    return (int) code;
}

void Error::setCode(Code code, const UnsafeStringView& message)
{
    m_code = code;
    if (message.empty()) {
        m_message = UnsafeStringView(codeName(code));
    } else {
        m_message = message;
    }
}

void Error::setSystemCode(int systemCode, Code codeIfUnresolved, const UnsafeStringView& message)
{
    Code code;
    switch (systemCode) {
    case EIO:
        code = Code::IOError;
        break;
    case ENOMEM:
        code = Code::NoMemory;
        break;
    case EACCES:
        code = Code::Permission;
        break;
    case EBUSY:
        code = Code::Busy;
        break;
    case ENOSPC:
        code = Code::Full;
        break;
    case EAUTH:
        code = Code::Authorization;
        break;
    case ENOENT:
        code = Code::NotFound;
        break;
    default:
        code = codeIfUnresolved;
        break;
    }
    setCode(code, message.empty() ? strerror(systemCode) : message);
    infos.insert_or_assign(ErrorStringKeySource, ErrorSourceSystem);
    infos.insert_or_assign(ErrorIntKeyExtCode, systemCode);
}

#ifdef _WIN32
void Error::setWinSystemCode(int systemCode, Code code, const UnsafeStringView& message)
{
    setCode(code, message.empty() ? std::system_category().message(systemCode).data() : message);
    infos.insert_or_assign(ErrorStringKeySource, ErrorSourceSystem);
    infos.insert_or_assign(ErrorIntKeyExtCode, systemCode);
}
#endif

void Error::setSQLiteCode(int rc, const UnsafeStringView& message)
{
    Code code = rc2c(rc);
    setCode(code, message.empty() ? sqlite3_errstr(rc) : message);
    if (c2rc(code) == rc) {
        infos.erase(ErrorIntKeyExtCode);
    } else {
        infos.insert_or_assign(ErrorIntKeyExtCode, rc);
    }
    infos.insert_or_assign(ErrorStringKeySource, ErrorSourceSQLite);
    if (code == Code::CantOpen || code == Code::IOError) {
#ifndef _WIN32
        infos.insert_or_assign("SystemErrno", errno);
        infos.insert_or_assign("SystemErrMsg", strerror(errno));
#else
        int err = GetLastError();
        infos.insert_or_assign("SystemErrno", err);
        infos.insert_or_assign("SystemErrMsg",
                               std::system_category().message(err).data());
#endif
    }
}

Error::Code Error::code() const
{
    return m_code;
}

bool Error::isOK() const
{
    return m_code == Code::OK;
}

bool Error::isCorruption() const
{
    return m_code == Error::Code::Corrupt || m_code == Error::Code::NotADatabase;
}

#pragma mark - ExtCode
Error::ExtCode Error::rc2ec(int rc)
{
    return (Error::ExtCode) rc;
}

const StringView& Error::getMessage() const
{
    return m_message;
}

#pragma mark - Common Info
UnsafeStringView Error::getPath() const
{
    auto iter = infos.find(ErrorStringKeyPath);
    if (iter != infos.end()) {
        return iter->second.textValue();
    }
    return UnsafeStringView();
}

UnsafeStringView Error::getAssociatePath() const
{
    auto iter = infos.find(ErrorStringKeyAssociatePath);
    if (iter != infos.end()) {
        return iter->second.textValue();
    }
    return UnsafeStringView();
}

UnsafeStringView Error::getSQL() const
{
    auto iter = infos.find(ErrorStringKeySQL);
    if (iter != infos.end()) {
        return iter->second.textValue();
    }
    return UnsafeStringView();
}

Tag Error::getTag() const
{
    auto iter = infos.find(ErrorIntKeyTag);
    if (iter != infos.end()) {
        return iter->second.intValue();
    }
    return Tag::invalid();
}

Error::ExtCode Error::getExtCode() const
{
    auto iter = infos.find(ErrorIntKeyExtCode);
    if (iter != infos.end()) {
        return rc2ec((int) iter->second.intValue());
    }
    return rc2ec(0);
}

#pragma mark - Description
StringView Error::getDescription() const
{
    WCTAssert(isError((int) code()));
    //    WCTAssert(!m_message.empty());
    std::ostringstream stream;
    stream << "[" << levelName(level) << ": " << (int) code() << ", " << m_message << "]";

    for (const auto& info : infos) {
        stream << ", " << info.first;
        switch (info.second.getType()) {
        case Value::Type::Text:
            stream << ":" << info.second.textValue();
            break;
        case Value::Type::Integer:
            stream << ":" << info.second.intValue();
            break;
        case Value::Type::Float:
            stream << ":" << info.second.floatValue();
            break;
        default:
            break;
        }
    }
    return StringView(stream.str());
}

} //namespace WCDB
