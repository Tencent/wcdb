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

#import <WCDB/Core.h>
#import <WCDB/WCTError.h>

@interface WCTError ()

- (instancetype)initWithError:(const WCDB::Error &)error;

@end

static_assert((int) WCTErrorLevelIgnore == (int) WCDB::Error::Level::Ignore, "");
static_assert((int) WCTErrorLevelDebug == (int) WCDB::Error::Level::Debug, "");
static_assert((int) WCTErrorLevelWarning == (int) WCDB::Error::Level::Warning, "");
static_assert((int) WCTErrorLevelError == (int) WCDB::Error::Level::Error, "");
static_assert((int) WCTErrorLevelNotice == (int) WCDB::Error::Level::Notice, "");
static_assert((int) WCTErrorLevelFatal == (int) WCDB::Error::Level::Fatal, "");

static_assert((int) WCTErrorCodeOK == (int) WCDB::Error::Code::OK, "");
static_assert((int) WCTErrorCodeError == (int) WCDB::Error::Code::Error, "");
static_assert((int) WCTErrorCodeInternal == (int) WCDB::Error::Code::Internal, "");
static_assert((int) WCTErrorCodePermission == (int) WCDB::Error::Code::Permission, "");
static_assert((int) WCTErrorCodeAbort == (int) WCDB::Error::Code::Abort, "");
static_assert((int) WCTErrorCodeBusy == (int) WCDB::Error::Code::Busy, "");
static_assert((int) WCTErrorCodeLocked == (int) WCDB::Error::Code::Locked, "");
static_assert((int) WCTErrorCodeNoMemory == (int) WCDB::Error::Code::NoMemory, "");
static_assert((int) WCTErrorCodeReadonly == (int) WCDB::Error::Code::Readonly, "");
static_assert((int) WCTErrorCodeInterrupt == (int) WCDB::Error::Code::Interrupt, "");
static_assert((int) WCTErrorCodeIOError == (int) WCDB::Error::Code::IOError, "");
static_assert((int) WCTErrorCodeCorrupt == (int) WCDB::Error::Code::Corrupt, "");
static_assert((int) WCTErrorCodeNotFound == (int) WCDB::Error::Code::NotFound, "");
static_assert((int) WCTErrorCodeFull == (int) WCDB::Error::Code::Full, "");
static_assert((int) WCTErrorCodeCantOpen == (int) WCDB::Error::Code::CantOpen, "");
static_assert((int) WCTErrorCodeProtocol == (int) WCDB::Error::Code::Protocol, "");
static_assert((int) WCTErrorCodeEmpty == (int) WCDB::Error::Code::Empty, "");
static_assert((int) WCTErrorCodeSchema == (int) WCDB::Error::Code::Schema, "");
static_assert((int) WCTErrorCodeExceed == (int) WCDB::Error::Code::Exceed, "");
static_assert((int) WCTErrorCodeConflict == (int) WCDB::Error::Code::Conflict, "");
static_assert((int) WCTErrorCodeMismatch == (int) WCDB::Error::Code::Mismatch, "");
static_assert((int) WCTErrorCodeMisuse == (int) WCDB::Error::Code::Misuse, "");
static_assert((int) WCTErrorCodeNoLargeFileSupport == (int) WCDB::Error::Code::NoLargeFileSupport, "");
static_assert((int) WCTErrorCodeAuthorization == (int) WCDB::Error::Code::Authorization, "");
static_assert((int) WCTErrorCodeFormat == (int) WCDB::Error::Code::Format, "");
static_assert((int) WCTErrorCodeRange == (int) WCDB::Error::Code::Range, "");
static_assert((int) WCTErrorCodeNotADatabase == (int) WCDB::Error::Code::NotADatabase, "");
static_assert((int) WCTErrorCodeNotice == (int) WCDB::Error::Code::Notice, "");
static_assert((int) WCTErrorCodeWarning == (int) WCDB::Error::Code::Warning, "");
static_assert((int) WCTErrorCodeRow == (int) WCDB::Error::Code::Row, "");
static_assert((int) WCTErrorCodeDone == (int) WCDB::Error::Code::Done, "");

static_assert((int) WCTInvalidTag == (int) WCDB::Tag::invalidValue, "");
static_assert(sizeof(WCTTag) == sizeof(WCDB::Tag), "");
