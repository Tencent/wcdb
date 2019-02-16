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

#import <WCDB/WCTDeclaration.h>

NS_ASSUME_NONNULL_BEGIN

WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeyPath;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeySQL;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeyOperation;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeyTag;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeyExtendedCode;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeySource;

typedef NS_ENUM(NSUInteger, WCTErrorLevel) {
    WCTErrorLevelIgnore = 1,
    WCTErrorLevelDebug = 2,
    WCTErrorLevelWarning = 3,
    WCTErrorLevelNotice = 4,
    WCTErrorLevelError = 5,
    WCTErrorLevelFatal = 6,
};

typedef NS_ENUM(NSUInteger, WCTErrorCode) {
    WCTErrorCodeOK = 0,
    WCTErrorCodeError = 1,
    WCTErrorCodeInternal = 2,
    WCTErrorCodePermission = 3,
    WCTErrorCodeAbort = 4,
    WCTErrorCodeBusy = 5,
    WCTErrorCodeLocked = 6,
    WCTErrorCodeNoMemory = 7,
    WCTErrorCodeReadonly = 8,
    WCTErrorCodeInterrupt = 9, /* Operation terminated by sqlite3_interrupt()*/
    WCTErrorCodeIOError = 10,
    WCTErrorCodeCorrupt = 11,
    WCTErrorCodeNotFound = 12,
    WCTErrorCodeFull = 13,
    WCTErrorCodeCantOpen = 14,
    WCTErrorCodeProtocol = 15,
    WCTErrorCodeEmpty = 16,
    WCTErrorCodeSchema = 17,
    WCTErrorCodeExceed = 18,
    WCTErrorCodeConstraint = 19,
    WCTErrorCodeMismatch = 20,
    WCTErrorCodeMisuse = 21,
    WCTErrorCodeNoLargeFileSupport = 22,
    WCTErrorCodeAuthorization = 23,
    WCTErrorCodeFormat = 24,
    WCTErrorCodeRange = 25,
    WCTErrorCodeNotADatabase = 26,
    WCTErrorCodeNotice = 27,
    WCTErrorCodeWarning = 28,
    WCTErrorCodeRow = 100,
    WCTErrorCodeDone = 101,
};

typedef NS_ENUM(NSUInteger, WCTErrorExtendedCode) {
    WCTErrorExtendedCodeErrorMissingCollseq = ((NSUInteger) WCTErrorCodeError | (1 << 8)),
    WCTErrorExtendedCodeErrorRetry = ((NSUInteger) WCTErrorCodeError | (2 << 8)),
    WCTErrorExtendedCodeErrorSnapshot = ((NSUInteger) WCTErrorCodeError | (3 << 8)),
    WCTErrorExtendedCodeIOErrorRead = ((NSUInteger) WCTErrorCodeIOError | (1 << 8)),
    WCTErrorExtendedCodeIOErrorShortRead = ((NSUInteger) WCTErrorCodeIOError | (2 << 8)),
    WCTErrorExtendedCodeIOErrorWrite = ((NSUInteger) WCTErrorCodeIOError | (3 << 8)),
    WCTErrorExtendedCodeIOErrorFsync = ((NSUInteger) WCTErrorCodeIOError | (4 << 8)),
    WCTErrorExtendedCodeIOErrorDirFsync = ((NSUInteger) WCTErrorCodeIOError | (5 << 8)),
    WCTErrorExtendedCodeIOErrorTruncate = ((NSUInteger) WCTErrorCodeIOError | (6 << 8)),
    WCTErrorExtendedCodeIOErrorFstat = ((NSUInteger) WCTErrorCodeIOError | (7 << 8)),
    WCTErrorExtendedCodeIOErrorUnlock = ((NSUInteger) WCTErrorCodeIOError | (8 << 8)),
    WCTErrorExtendedCodeIOErrorRdlock = ((NSUInteger) WCTErrorCodeIOError | (9 << 8)),
    WCTErrorExtendedCodeIOErrorDelete = ((NSUInteger) WCTErrorCodeIOError | (10 << 8)),
    WCTErrorExtendedCodeIOErrorBlocked = ((NSUInteger) WCTErrorCodeIOError | (11 << 8)),
    WCTErrorExtendedCodeIOErrorNoMemory = ((NSUInteger) WCTErrorCodeIOError | (12 << 8)),
    WCTErrorExtendedCodeIOErrorAccess = ((NSUInteger) WCTErrorCodeIOError | (13 << 8)),
    WCTErrorExtendedCodeIOErrorCheckReservedLock = ((NSUInteger) WCTErrorCodeIOError | (14 << 8)),
    WCTErrorExtendedCodeIOErrorLock = ((NSUInteger) WCTErrorCodeIOError | (15 << 8)),
    WCTErrorExtendedCodeIOErrorClose = ((NSUInteger) WCTErrorCodeIOError | (16 << 8)),
    WCTErrorExtendedCodeIOErrorDirClose = ((NSUInteger) WCTErrorCodeIOError | (17 << 8)),
    WCTErrorExtendedCodeIOErrorShmOpen = ((NSUInteger) WCTErrorCodeIOError | (18 << 8)),
    WCTErrorExtendedCodeIOErrorShmSize = ((NSUInteger) WCTErrorCodeIOError | (19 << 8)),
    WCTErrorExtendedCodeIOErrorShmLock = ((NSUInteger) WCTErrorCodeIOError | (20 << 8)),
    WCTErrorExtendedCodeIOErrorShmMap = ((NSUInteger) WCTErrorCodeIOError | (21 << 8)),
    WCTErrorExtendedCodeIOErrorSeek = ((NSUInteger) WCTErrorCodeIOError | (22 << 8)),
    WCTErrorExtendedCodeIOErrorDeleteNoEntry = ((NSUInteger) WCTErrorCodeIOError | (23 << 8)),
    WCTErrorExtendedCodeIOErrorMmap = ((NSUInteger) WCTErrorCodeIOError | (24 << 8)),
    WCTErrorExtendedCodeIOErrorGetTempPath = ((NSUInteger) WCTErrorCodeIOError | (25 << 8)),
    WCTErrorExtendedCodeIOErrorConvPath = ((NSUInteger) WCTErrorCodeIOError | (26 << 8)),
    WCTErrorExtendedCodeIOErrorVnode = ((NSUInteger) WCTErrorCodeIOError | (27 << 8)),
    WCTErrorExtendedCodeIOErrorAuthorization = ((NSUInteger) WCTErrorCodeIOError | (28 << 8)),
    WCTErrorExtendedCodeIOErrorBeginAtomic = ((NSUInteger) WCTErrorCodeIOError | (29 << 8)),
    WCTErrorExtendedCodeIOErrorCommitAtomic = ((NSUInteger) WCTErrorCodeIOError | (30 << 8)),
    WCTErrorExtendedCodeIOErrorRollbackAtomic = ((NSUInteger) WCTErrorCodeIOError | (31 << 8)),
    WCTErrorExtendedCodeLockedSharedCache = ((NSUInteger) WCTErrorCodeLocked | (1 << 8)),
    WCTErrorExtendedCodeLockedVirtualTable = ((NSUInteger) WCTErrorCodeLocked | (2 << 8)),
    WCTErrorExtendedCodeBusyRecovery = ((NSUInteger) WCTErrorCodeBusy | (1 << 8)),
    WCTErrorExtendedCodeBusySnapshot = ((NSUInteger) WCTErrorCodeBusy | (2 << 8)),
    WCTErrorExtendedCodeCantOpenNoTempDir = ((NSUInteger) WCTErrorCodeCantOpen | (1 << 8)),
    WCTErrorExtendedCodeCantOpenIsDir = ((NSUInteger) WCTErrorCodeCantOpen | (2 << 8)),
    WCTErrorExtendedCodeCantOpenFullPath = ((NSUInteger) WCTErrorCodeCantOpen | (3 << 8)),
    WCTErrorExtendedCodeCantOpenConvPath = ((NSUInteger) WCTErrorCodeCantOpen | (4 << 8)),
    WCTErrorExtendedCodeCantOpenDirtyWal = ((NSUInteger) WCTErrorCodeCantOpen | (5 << 8)),
    WCTErrorExtendedCodeCorruptVirtualTable = ((NSUInteger) WCTErrorCodeCorrupt | (1 << 8)),
    WCTErrorExtendedCodeCorruptSequence = ((NSUInteger) WCTErrorCodeCorrupt | (2 << 8)),
    WCTErrorExtendedCodeReadonlyRecovery = ((NSUInteger) WCTErrorCodeReadonly | (1 << 8)),
    WCTErrorExtendedCodeReadonlyCantLock = ((NSUInteger) WCTErrorCodeReadonly | (2 << 8)),
    WCTErrorExtendedCodeReadonlyRollback = ((NSUInteger) WCTErrorCodeReadonly | (3 << 8)),
    WCTErrorExtendedCodeReadonlyDatabaseMoved = ((NSUInteger) WCTErrorCodeReadonly | (4 << 8)),
    WCTErrorExtendedCodeReadonlyCantInit = ((NSUInteger) WCTErrorCodeReadonly | (5 << 8)),
    WCTErrorExtendedCodeReadonlyDirectory = ((NSUInteger) WCTErrorCodeReadonly | (6 << 8)),
    WCTErrorExtendedCodeAbortRollback = ((NSUInteger) WCTErrorCodeAbort | (2 << 8)),
    WCTErrorExtendedCodeConstraintCheck = ((NSUInteger) WCTErrorCodeConstraint | (1 << 8)),
    WCTErrorExtendedCodeConstraintCommitHook = ((NSUInteger) WCTErrorCodeConstraint | (2 << 8)),
    WCTErrorExtendedCodeConstraintForeignKey = ((NSUInteger) WCTErrorCodeConstraint | (3 << 8)),
    WCTErrorExtendedCodeConstraintFunction = ((NSUInteger) WCTErrorCodeConstraint | (4 << 8)),
    WCTErrorExtendedCodeConstraintNotNull = ((NSUInteger) WCTErrorCodeConstraint | (5 << 8)),
    WCTErrorExtendedCodeConstraintPrimaryKey = ((NSUInteger) WCTErrorCodeConstraint | (6 << 8)),
    WCTErrorExtendedCodeConstraintTrigger = ((NSUInteger) WCTErrorCodeConstraint | (7 << 8)),
    WCTErrorExtendedCodeConstraintUnique = ((NSUInteger) WCTErrorCodeConstraint | (8 << 8)),
    WCTErrorExtendedCodeConstraintVirtualTable = ((NSUInteger) WCTErrorCodeConstraint | (9 << 8)),
    WCTErrorExtendedCodeConstraintRowID = ((NSUInteger) WCTErrorCodeConstraint | (10 << 8)),
    WCTErrorExtendedCodeNoticeRecoverWal = ((NSUInteger) WCTErrorCodeNotice | (1 << 8)),
    WCTErrorExtendedCodeNoticeRecoverRollback = ((NSUInteger) WCTErrorCodeNotice | (2 << 8)),
    WCTErrorExtendedCodeWarningAutoIndex = ((NSUInteger) WCTErrorCodeWarning | (1 << 8)),
    WCTErrorExtendedCodeAuthorizationUser = ((NSUInteger) WCTErrorCodeAuthorization | (1 << 8)),
    WCTErrorExtendedCodeOKLoadPermanently = ((NSUInteger) WCTErrorCodeOK | (1 << 8)),
};

@interface WCTError : NSError

- (instancetype)initWithCode:(WCTErrorCode)code
                       level:(WCTErrorLevel)level
                     message:(nonnull NSString *)message
                    userInfo:(nullable NSDictionary<NSErrorUserInfoKey, id> *)userInfo;

@property (nonatomic, readonly) WCTErrorLevel level;

@property (nonatomic, readonly, nullable) NSString *message;

- (BOOL)isOK;

- (BOOL)isCorruption;

@end

@interface WCTError (Path)

- (nullable NSString *)path;

@end

@interface WCTError (Tag)

- (WCTTag)tag;

@end

@interface WCTError (SQL)

- (nullable NSString *)sql;

@end

@interface WCTError (ExtendedCode)

- (WCTErrorExtendedCode)extendedCode;

@end

@interface WCTError (Source)

- (nullable NSString *)source;

@end

NS_ASSUME_NONNULL_END
