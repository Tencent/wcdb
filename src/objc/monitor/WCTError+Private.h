//
// Created by sanhuazhang on 2019/05/02
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

#import "Core.h"
#import "CoreConst.h"
#import "WCTError.h"

@interface WCTError ()

- (instancetype)initWithError:(const WCDB::Error &)error;

@end

static_assert((NSUInteger) WCTErrorLevelIgnore == (NSUInteger) WCDB::Error::Level::Ignore, "");
static_assert((NSUInteger) WCTErrorLevelDebug == (NSUInteger) WCDB::Error::Level::Debug, "");
static_assert((NSUInteger) WCTErrorLevelWarning == (NSUInteger) WCDB::Error::Level::Warning, "");
static_assert((NSUInteger) WCTErrorLevelError == (NSUInteger) WCDB::Error::Level::Error, "");
static_assert((NSUInteger) WCTErrorLevelNotice == (NSUInteger) WCDB::Error::Level::Notice, "");
static_assert((NSUInteger) WCTErrorLevelFatal == (NSUInteger) WCDB::Error::Level::Fatal, "");

static_assert((NSUInteger) WCTErrorCodeOK == (NSUInteger) WCDB::Error::Code::OK, "");
static_assert((NSUInteger) WCTErrorCodeError == (NSUInteger) WCDB::Error::Code::Error, "");
static_assert((NSUInteger) WCTErrorCodeInternal == (NSUInteger) WCDB::Error::Code::Internal, "");
static_assert((NSUInteger) WCTErrorCodePermission == (NSUInteger) WCDB::Error::Code::Permission, "");
static_assert((NSUInteger) WCTErrorCodeAbort == (NSUInteger) WCDB::Error::Code::Abort, "");
static_assert((NSUInteger) WCTErrorCodeBusy == (NSUInteger) WCDB::Error::Code::Busy, "");
static_assert((NSUInteger) WCTErrorCodeLocked == (NSUInteger) WCDB::Error::Code::Locked, "");
static_assert((NSUInteger) WCTErrorCodeNoMemory == (NSUInteger) WCDB::Error::Code::NoMemory, "");
static_assert((NSUInteger) WCTErrorCodeReadonly == (NSUInteger) WCDB::Error::Code::Readonly, "");
static_assert((NSUInteger) WCTErrorCodeInterrupt == (NSUInteger) WCDB::Error::Code::Interrupt, "");
static_assert((NSUInteger) WCTErrorCodeIOError == (NSUInteger) WCDB::Error::Code::IOError, "");
static_assert((NSUInteger) WCTErrorCodeCorrupt == (NSUInteger) WCDB::Error::Code::Corrupt, "");
static_assert((NSUInteger) WCTErrorCodeNotFound == (NSUInteger) WCDB::Error::Code::NotFound, "");
static_assert((NSUInteger) WCTErrorCodeFull == (NSUInteger) WCDB::Error::Code::Full, "");
static_assert((NSUInteger) WCTErrorCodeCantOpen == (NSUInteger) WCDB::Error::Code::CantOpen, "");
static_assert((NSUInteger) WCTErrorCodeProtocol == (NSUInteger) WCDB::Error::Code::Protocol, "");
static_assert((NSUInteger) WCTErrorCodeEmpty == (NSUInteger) WCDB::Error::Code::Empty, "");
static_assert((NSUInteger) WCTErrorCodeSchema == (NSUInteger) WCDB::Error::Code::Schema, "");
static_assert((NSUInteger) WCTErrorCodeExceed == (NSUInteger) WCDB::Error::Code::Exceed, "");
static_assert((NSUInteger) WCTErrorCodeConstraint == (NSUInteger) WCDB::Error::Code::Constraint, "");
static_assert((NSUInteger) WCTErrorCodeMismatch == (NSUInteger) WCDB::Error::Code::Mismatch, "");
static_assert((NSUInteger) WCTErrorCodeMisuse == (NSUInteger) WCDB::Error::Code::Misuse, "");
static_assert((NSUInteger) WCTErrorCodeNoLargeFileSupport == (NSUInteger) WCDB::Error::Code::NoLargeFileSupport, "");
static_assert((NSUInteger) WCTErrorCodeAuthorization == (NSUInteger) WCDB::Error::Code::Authorization, "");
static_assert((NSUInteger) WCTErrorCodeFormat == (NSUInteger) WCDB::Error::Code::Format, "");
static_assert((NSUInteger) WCTErrorCodeRange == (NSUInteger) WCDB::Error::Code::Range, "");
static_assert((NSUInteger) WCTErrorCodeNotADatabase == (NSUInteger) WCDB::Error::Code::NotADatabase, "");
static_assert((NSUInteger) WCTErrorCodeNotice == (NSUInteger) WCDB::Error::Code::Notice, "");
static_assert((NSUInteger) WCTErrorCodeWarning == (NSUInteger) WCDB::Error::Code::Warning, "");
static_assert((NSUInteger) WCTErrorCodeRow == (NSUInteger) WCDB::Error::Code::Row, "");
static_assert((NSUInteger) WCTErrorCodeDone == (NSUInteger) WCDB::Error::Code::Done, "");

static_assert((NSUInteger) WCTErrorExtendedCodeErrorMissingCollseq == (NSUInteger) WCDB::Error::ExtCode::ErrorMissingCollseq, "");
static_assert((NSUInteger) WCTErrorExtendedCodeErrorRetry == (NSUInteger) WCDB::Error::ExtCode::ErrorRetry, "");
static_assert((NSUInteger) WCTErrorExtendedCodeErrorSnapshot == (NSUInteger) WCDB::Error::ExtCode::ErrorSnapshot, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorRead == (NSUInteger) WCDB::Error::ExtCode::IOErrorRead, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorShortRead == (NSUInteger) WCDB::Error::ExtCode::IOErrorShortRead, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorWrite == (NSUInteger) WCDB::Error::ExtCode::IOErrorWrite, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorFsync == (NSUInteger) WCDB::Error::ExtCode::IOErrorFsync, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorDirFsync == (NSUInteger) WCDB::Error::ExtCode::IOErrorDirFsync, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorTruncate == (NSUInteger) WCDB::Error::ExtCode::IOErrorTruncate, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorFstat == (NSUInteger) WCDB::Error::ExtCode::IOErrorFstat, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorUnlock == (NSUInteger) WCDB::Error::ExtCode::IOErrorUnlock, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorRdlock == (NSUInteger) WCDB::Error::ExtCode::IOErrorRdlock, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorDelete == (NSUInteger) WCDB::Error::ExtCode::IOErrorDelete, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorBlocked == (NSUInteger) WCDB::Error::ExtCode::IOErrorBlocked, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorNoMemory == (NSUInteger) WCDB::Error::ExtCode::IOErrorNoMemory, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorAccess == (NSUInteger) WCDB::Error::ExtCode::IOErrorAccess, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorCheckReservedLock == (NSUInteger) WCDB::Error::ExtCode::IOErrorCheckReservedLock, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorLock == (NSUInteger) WCDB::Error::ExtCode::IOErrorLock, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorClose == (NSUInteger) WCDB::Error::ExtCode::IOErrorClose, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorDirClose == (NSUInteger) WCDB::Error::ExtCode::IOErrorDirClose, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorShmOpen == (NSUInteger) WCDB::Error::ExtCode::IOErrorShmOpen, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorShmSize == (NSUInteger) WCDB::Error::ExtCode::IOErrorShmSize, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorShmLock == (NSUInteger) WCDB::Error::ExtCode::IOErrorShmLock, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorShmMap == (NSUInteger) WCDB::Error::ExtCode::IOErrorShmMap, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorSeek == (NSUInteger) WCDB::Error::ExtCode::IOErrorSeek, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorDeleteNoEntry == (NSUInteger) WCDB::Error::ExtCode::IOErrorDeleteNoEntry, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorMmap == (NSUInteger) WCDB::Error::ExtCode::IOErrorMmap, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorGetTempPath == (NSUInteger) WCDB::Error::ExtCode::IOErrorGetTempPath, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorConvPath == (NSUInteger) WCDB::Error::ExtCode::IOErrorConvPath, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorVnode == (NSUInteger) WCDB::Error::ExtCode::IOErrorVnode, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorAuthorization == (NSUInteger) WCDB::Error::ExtCode::IOErrorAuthorization, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorBeginAtomic == (NSUInteger) WCDB::Error::ExtCode::IOErrorBeginAtomic, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorCommitAtomic == (NSUInteger) WCDB::Error::ExtCode::IOErrorCommitAtomic, "");
static_assert((NSUInteger) WCTErrorExtendedCodeIOErrorRollbackAtomic == (NSUInteger) WCDB::Error::ExtCode::IOErrorRollbackAtomic, "");
static_assert((NSUInteger) WCTErrorExtendedCodeLockedSharedCache == (NSUInteger) WCDB::Error::ExtCode::LockedSharedCache, "");
static_assert((NSUInteger) WCTErrorExtendedCodeLockedVirtualTable == (NSUInteger) WCDB::Error::ExtCode::LockedVirtualTable, "");
static_assert((NSUInteger) WCTErrorExtendedCodeBusyRecovery == (NSUInteger) WCDB::Error::ExtCode::BusyRecovery, "");
static_assert((NSUInteger) WCTErrorExtendedCodeBusySnapshot == (NSUInteger) WCDB::Error::ExtCode::BusySnapshot, "");
static_assert((NSUInteger) WCTErrorExtendedCodeCantOpenNoTempDir == (NSUInteger) WCDB::Error::ExtCode::CantOpenNoTempDir, "");
static_assert((NSUInteger) WCTErrorExtendedCodeCantOpenIsDir == (NSUInteger) WCDB::Error::ExtCode::CantOpenIsDir, "");
static_assert((NSUInteger) WCTErrorExtendedCodeCantOpenFullPath == (NSUInteger) WCDB::Error::ExtCode::CantOpenFullPath, "");
static_assert((NSUInteger) WCTErrorExtendedCodeCantOpenConvPath == (NSUInteger) WCDB::Error::ExtCode::CantOpenConvPath, "");
static_assert((NSUInteger) WCTErrorExtendedCodeCantOpenDirtyWal == (NSUInteger) WCDB::Error::ExtCode::CantOpenDirtyWal, "");
static_assert((NSUInteger) WCTErrorExtendedCodeCorruptVirtualTable == (NSUInteger) WCDB::Error::ExtCode::CorruptVirtualTable, "");
static_assert((NSUInteger) WCTErrorExtendedCodeCorruptSequence == (NSUInteger) WCDB::Error::ExtCode::CorruptSequence, "");
static_assert((NSUInteger) WCTErrorExtendedCodeReadonlyRecovery == (NSUInteger) WCDB::Error::ExtCode::ReadonlyRecovery, "");
static_assert((NSUInteger) WCTErrorExtendedCodeReadonlyCantLock == (NSUInteger) WCDB::Error::ExtCode::ReadonlyCantLock, "");
static_assert((NSUInteger) WCTErrorExtendedCodeReadonlyRollback == (NSUInteger) WCDB::Error::ExtCode::ReadonlyRollback, "");
static_assert((NSUInteger) WCTErrorExtendedCodeReadonlyDatabaseMoved == (NSUInteger) WCDB::Error::ExtCode::ReadonlyDatabaseMoved, "");
static_assert((NSUInteger) WCTErrorExtendedCodeReadonlyCantInit == (NSUInteger) WCDB::Error::ExtCode::ReadonlyCantInit, "");
static_assert((NSUInteger) WCTErrorExtendedCodeReadonlyDirectory == (NSUInteger) WCDB::Error::ExtCode::ReadonlyDirectory, "");
static_assert((NSUInteger) WCTErrorExtendedCodeAbortRollback == (NSUInteger) WCDB::Error::ExtCode::AbortRollback, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintCheck == (NSUInteger) WCDB::Error::ExtCode::ConstraintCheck, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintCommitHook == (NSUInteger) WCDB::Error::ExtCode::ConstraintCommitHook, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintForeignKey == (NSUInteger) WCDB::Error::ExtCode::ConstraintForeignKey, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintFunction == (NSUInteger) WCDB::Error::ExtCode::ConstraintFunction, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintNotNull == (NSUInteger) WCDB::Error::ExtCode::ConstraintNotNull, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintPrimaryKey == (NSUInteger) WCDB::Error::ExtCode::ConstraintPrimaryKey, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintTrigger == (NSUInteger) WCDB::Error::ExtCode::ConstraintTrigger, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintUnique == (NSUInteger) WCDB::Error::ExtCode::ConstraintUnique, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintVirtualTable == (NSUInteger) WCDB::Error::ExtCode::ConstraintVirtualTable, "");
static_assert((NSUInteger) WCTErrorExtendedCodeConstraintRowID == (NSUInteger) WCDB::Error::ExtCode::ConstraintRowID, "");
static_assert((NSUInteger) WCTErrorExtendedCodeNoticeRecoverWal == (NSUInteger) WCDB::Error::ExtCode::NoticeRecoverWal, "");
static_assert((NSUInteger) WCTErrorExtendedCodeNoticeRecoverRollback == (NSUInteger) WCDB::Error::ExtCode::NoticeRecoverRollback, "");
static_assert((NSUInteger) WCTErrorExtendedCodeWarningAutoIndex == (NSUInteger) WCDB::Error::ExtCode::WarningAutoIndex, "");
static_assert((NSUInteger) WCTErrorExtendedCodeAuthorizationUser == (NSUInteger) WCDB::Error::ExtCode::AuthorizationUser, "");
static_assert((NSUInteger) WCTErrorExtendedCodeOKLoadPermanently == (NSUInteger) WCDB::Error::ExtCode::OKLoadPermanently, "");

static_assert((int) WCTInvalidTag == (int) WCDB::TagInvalidValue, "");
static_assert(sizeof(WCTTag) == sizeof(WCDB::Tag), "");
