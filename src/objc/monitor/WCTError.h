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

#import "Macro.h"
#import "WCTMacroUtility.h"
#import "WCTTag.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

WCDB_EXTERN NSErrorDomain const WCTErrorDomain;

WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeyPath;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeySQL;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeyTag;
WCDB_EXTERN NSErrorUserInfoKey const WCTErrorKeyExtendedCode;

typedef NS_ENUM(NSUInteger, WCTErrorLevel) {
    WCTErrorLevelIgnore = 1,
    WCTErrorLevelDebug = 2,
    WCTErrorLevelNotice = 3,
    WCTErrorLevelWarning = 4,
    WCTErrorLevelError = 5, // Only for the errors that will cause api to return false.
    WCTErrorLevelFatal = 6, // Application should abort.
};

typedef NS_ENUM(NSUInteger, WCTErrorCode) {
    /**
     * Error code for sqlite. You can check it at http://www.sqlite.org/rescode.html .
     */
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
    WCTErrorCodeZstdError = 1000,
};

typedef NS_ENUM(NSUInteger, WCTErrorExtendedCode) {
    /**
     * Extended error code for sqlite. You can check it at http://www.sqlite.org/rescode.html .
     */
    WCTErrorExtendedCodeErrorMissingCollseq = ((NSUInteger) WCTErrorCodeError | (1 << 8)),         // 257
    WCTErrorExtendedCodeErrorRetry = ((NSUInteger) WCTErrorCodeError | (2 << 8)),                  // 513
    WCTErrorExtendedCodeErrorSnapshot = ((NSUInteger) WCTErrorCodeError | (3 << 8)),               // 769
    WCTErrorExtendedCodeIOErrorRead = ((NSUInteger) WCTErrorCodeIOError | (1 << 8)),               // 266
    WCTErrorExtendedCodeIOErrorShortRead = ((NSUInteger) WCTErrorCodeIOError | (2 << 8)),          // 522
    WCTErrorExtendedCodeIOErrorWrite = ((NSUInteger) WCTErrorCodeIOError | (3 << 8)),              // 778
    WCTErrorExtendedCodeIOErrorFsync = ((NSUInteger) WCTErrorCodeIOError | (4 << 8)),              // 1034
    WCTErrorExtendedCodeIOErrorDirFsync = ((NSUInteger) WCTErrorCodeIOError | (5 << 8)),           // 1290
    WCTErrorExtendedCodeIOErrorTruncate = ((NSUInteger) WCTErrorCodeIOError | (6 << 8)),           // 1546
    WCTErrorExtendedCodeIOErrorFstat = ((NSUInteger) WCTErrorCodeIOError | (7 << 8)),              // 1802
    WCTErrorExtendedCodeIOErrorUnlock = ((NSUInteger) WCTErrorCodeIOError | (8 << 8)),             // 2058
    WCTErrorExtendedCodeIOErrorRdlock = ((NSUInteger) WCTErrorCodeIOError | (9 << 8)),             // 2314
    WCTErrorExtendedCodeIOErrorDelete = ((NSUInteger) WCTErrorCodeIOError | (10 << 8)),            // 2570
    WCTErrorExtendedCodeIOErrorBlocked = ((NSUInteger) WCTErrorCodeIOError | (11 << 8)),           // 2826
    WCTErrorExtendedCodeIOErrorNoMemory = ((NSUInteger) WCTErrorCodeIOError | (12 << 8)),          // 3082
    WCTErrorExtendedCodeIOErrorAccess = ((NSUInteger) WCTErrorCodeIOError | (13 << 8)),            // 3338
    WCTErrorExtendedCodeIOErrorCheckReservedLock = ((NSUInteger) WCTErrorCodeIOError | (14 << 8)), // 3594
    WCTErrorExtendedCodeIOErrorLock = ((NSUInteger) WCTErrorCodeIOError | (15 << 8)),              // 3850
    WCTErrorExtendedCodeIOErrorClose = ((NSUInteger) WCTErrorCodeIOError | (16 << 8)),             // 4106
    WCTErrorExtendedCodeIOErrorDirClose = ((NSUInteger) WCTErrorCodeIOError | (17 << 8)),          // 4362
    WCTErrorExtendedCodeIOErrorShmOpen = ((NSUInteger) WCTErrorCodeIOError | (18 << 8)),           // 4618
    WCTErrorExtendedCodeIOErrorShmSize = ((NSUInteger) WCTErrorCodeIOError | (19 << 8)),           // 4874
    WCTErrorExtendedCodeIOErrorShmLock = ((NSUInteger) WCTErrorCodeIOError | (20 << 8)),           // 5130
    WCTErrorExtendedCodeIOErrorShmMap = ((NSUInteger) WCTErrorCodeIOError | (21 << 8)),            // 5386
    WCTErrorExtendedCodeIOErrorSeek = ((NSUInteger) WCTErrorCodeIOError | (22 << 8)),              // 5642
    WCTErrorExtendedCodeIOErrorDeleteNoEntry = ((NSUInteger) WCTErrorCodeIOError | (23 << 8)),     // 5898
    WCTErrorExtendedCodeIOErrorMmap = ((NSUInteger) WCTErrorCodeIOError | (24 << 8)),              // 6154
    WCTErrorExtendedCodeIOErrorGetTempPath = ((NSUInteger) WCTErrorCodeIOError | (25 << 8)),       // 6410
    WCTErrorExtendedCodeIOErrorConvPath = ((NSUInteger) WCTErrorCodeIOError | (26 << 8)),          // 6666
    WCTErrorExtendedCodeIOErrorVnode = ((NSUInteger) WCTErrorCodeIOError | (27 << 8)),             // 6922
    WCTErrorExtendedCodeIOErrorAuthorization = ((NSUInteger) WCTErrorCodeIOError | (28 << 8)),     // 7178
    WCTErrorExtendedCodeIOErrorBeginAtomic = ((NSUInteger) WCTErrorCodeIOError | (29 << 8)),       // 7434
    WCTErrorExtendedCodeIOErrorCommitAtomic = ((NSUInteger) WCTErrorCodeIOError | (30 << 8)),      // 7690
    WCTErrorExtendedCodeIOErrorRollbackAtomic = ((NSUInteger) WCTErrorCodeIOError | (31 << 8)),    // 7946
    WCTErrorExtendedCodeLockedSharedCache = ((NSUInteger) WCTErrorCodeLocked | (1 << 8)),          // 262
    WCTErrorExtendedCodeLockedVirtualTable = ((NSUInteger) WCTErrorCodeLocked | (2 << 8)),         // 518
    WCTErrorExtendedCodeBusyRecovery = ((NSUInteger) WCTErrorCodeBusy | (1 << 8)),                 // 261
    WCTErrorExtendedCodeBusySnapshot = ((NSUInteger) WCTErrorCodeBusy | (2 << 8)),                 // 517
    WCTErrorExtendedCodeCantOpenNoTempDir = ((NSUInteger) WCTErrorCodeCantOpen | (1 << 8)),        // 270
    WCTErrorExtendedCodeCantOpenIsDir = ((NSUInteger) WCTErrorCodeCantOpen | (2 << 8)),            // 526
    WCTErrorExtendedCodeCantOpenFullPath = ((NSUInteger) WCTErrorCodeCantOpen | (3 << 8)),         // 782
    WCTErrorExtendedCodeCantOpenConvPath = ((NSUInteger) WCTErrorCodeCantOpen | (4 << 8)),         // 1038
    WCTErrorExtendedCodeCantOpenDirtyWal = ((NSUInteger) WCTErrorCodeCantOpen | (5 << 8)),         // 1294
    WCTErrorExtendedCodeCorruptVirtualTable = ((NSUInteger) WCTErrorCodeCorrupt | (1 << 8)),       // 267
    WCTErrorExtendedCodeCorruptSequence = ((NSUInteger) WCTErrorCodeCorrupt | (2 << 8)),           // 523
    WCTErrorExtendedCodeReadonlyRecovery = ((NSUInteger) WCTErrorCodeReadonly | (1 << 8)),         // 264
    WCTErrorExtendedCodeReadonlyCantLock = ((NSUInteger) WCTErrorCodeReadonly | (2 << 8)),         // 520
    WCTErrorExtendedCodeReadonlyRollback = ((NSUInteger) WCTErrorCodeReadonly | (3 << 8)),         // 776
    WCTErrorExtendedCodeReadonlyDatabaseMoved = ((NSUInteger) WCTErrorCodeReadonly | (4 << 8)),    // 1032
    WCTErrorExtendedCodeReadonlyCantInit = ((NSUInteger) WCTErrorCodeReadonly | (5 << 8)),         // 1288
    WCTErrorExtendedCodeReadonlyDirectory = ((NSUInteger) WCTErrorCodeReadonly | (6 << 8)),        // 1544
    WCTErrorExtendedCodeAbortRollback = ((NSUInteger) WCTErrorCodeAbort | (2 << 8)),               // 516
    WCTErrorExtendedCodeConstraintCheck = ((NSUInteger) WCTErrorCodeConstraint | (1 << 8)),        // 275
    WCTErrorExtendedCodeConstraintCommitHook = ((NSUInteger) WCTErrorCodeConstraint | (2 << 8)),   // 531
    WCTErrorExtendedCodeConstraintForeignKey = ((NSUInteger) WCTErrorCodeConstraint | (3 << 8)),   // 787
    WCTErrorExtendedCodeConstraintFunction = ((NSUInteger) WCTErrorCodeConstraint | (4 << 8)),     // 1043
    WCTErrorExtendedCodeConstraintNotNull = ((NSUInteger) WCTErrorCodeConstraint | (5 << 8)),      // 1299
    WCTErrorExtendedCodeConstraintPrimaryKey = ((NSUInteger) WCTErrorCodeConstraint | (6 << 8)),   // 1555
    WCTErrorExtendedCodeConstraintTrigger = ((NSUInteger) WCTErrorCodeConstraint | (7 << 8)),      // 1811
    WCTErrorExtendedCodeConstraintUnique = ((NSUInteger) WCTErrorCodeConstraint | (8 << 8)),       // 2067
    WCTErrorExtendedCodeConstraintVirtualTable = ((NSUInteger) WCTErrorCodeConstraint | (9 << 8)), // 2323
    WCTErrorExtendedCodeConstraintRowID = ((NSUInteger) WCTErrorCodeConstraint | (10 << 8)),       // 2579
    WCTErrorExtendedCodeNoticeRecoverWal = ((NSUInteger) WCTErrorCodeNotice | (1 << 8)),           // 283
    WCTErrorExtendedCodeNoticeRecoverRollback = ((NSUInteger) WCTErrorCodeNotice | (2 << 8)),      // 539
    WCTErrorExtendedCodeWarningAutoIndex = ((NSUInteger) WCTErrorCodeWarning | (1 << 8)),          // 284
    WCTErrorExtendedCodeAuthorizationUser = ((NSUInteger) WCTErrorCodeAuthorization | (1 << 8)),   // 279
    WCTErrorExtendedCodeOKLoadPermanently = ((NSUInteger) WCTErrorCodeOK | (1 << 8)),              // 256
};

WCDB_API @interface WCTError : NSError

- (instancetype)initWithCode:(WCTErrorCode)code
                       level:(WCTErrorLevel)level
                     message:(nonnull NSString *)message
                    userInfo:(nullable NSDictionary<NSErrorUserInfoKey, id> *)userInfo NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithDomain:(NSErrorDomain)domain code:(NSInteger)code userInfo:(nullable NSDictionary<NSErrorUserInfoKey, id> *)dict UNAVAILABLE_ATTRIBUTE;

+ (instancetype)errorWithDomain:(NSErrorDomain)domain code:(NSInteger)code userInfo:(nullable NSDictionary<NSErrorUserInfoKey, id> *)dict UNAVAILABLE_ATTRIBUTE;

/**
 * Level of error.
 */
@property (nonatomic, readonly) WCTErrorLevel level;

/**
 * Text message that helps you debug it.
 */
@property (nonatomic, readonly) NSString *message;

/**
 @brief Convenient interface for checking code==0.
 @return YES for no error
 */
- (BOOL)isOK;

/**
 @brief Convenient interface for checking code==`WCTErrorCodeCorrupt` or code==`WCTErrorCodeNotADatabase`, which means database may be corrupted.
 @return YES for no error
 */
- (BOOL)isCorruption;

@end

WCDB_API @interface WCTError(Path)

/**
 * The file path of database that is being manipulated when an error occurs.
 */
- (nullable NSString *)path;

@end

WCDB_API @interface WCTError(Tag)

/**
 * The tag of database that is being manipulated when an error occurs.
 */
- (WCTTag)tag;

@end

WCDB_API @interface WCTError(SQL)

/**
 * The SQL is being executed when an error occurs.
 */
- (nullable NSString *)sql;

@end

WCDB_API @interface WCTError(ExtendedCode)

/**
 * Extended error code for sqlite. You can check it at http://www.sqlite.org/rescode.html .
 */
- (WCTErrorExtendedCode)extendedCode;

@end

NS_ASSUME_NONNULL_END
