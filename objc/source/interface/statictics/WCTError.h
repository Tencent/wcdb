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

#import <Foundation/Foundation.h>
#import <WCDB/WCTMacro.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, WCTErrorLevel) {
    WCTErrorLevelIgnore = 1,
    WCTErrorLevelDebug = 2,
    WCTErrorLevelWarning = 3,
    WCTErrorLevelNotice = 4,
    WCTErrorLevelError = 5,
    WCTErrorLevelFatal = 6,
};

typedef int32_t WCTTag;
constexpr WCTTag const WCTInvalidTag = 0;

typedef NS_ENUM(NSUInteger, WCTErrorCode) {
    WCTErrorCodeOK = 0,                  /* Successful result */
    WCTErrorCodeError = 1,               /* SQL error, missing database or other error */
    WCTErrorCodeInternal = 2,            /* Internal logic error */
    WCTErrorCodePermission = 3,          /* Access permission denied */
    WCTErrorCodeAbort = 4,               /* Callback routine requested an abort */
    WCTErrorCodeBusy = 5,                /* The database file is locked */
    WCTErrorCodeLocked = 6,              /* A table in the database is locked */
    WCTErrorCodeNoMemory = 7,            /* Out of memory */
    WCTErrorCodeReadonly = 8,            /* Attempt to write a readonly file */
    WCTErrorCodeInterrupt = 9,           /* Operation terminated by sqlite3_interrupt()*/
    WCTErrorCodeIOError = 10,            /* Some kind of disk I/O error occurred */
    WCTErrorCodeCorrupt = 11,            /* The data(database, memory or other files) acquired is malformed */
    WCTErrorCodeNotFound = 12,           /* Unknown opcode in sqlite3_file_control() */
    WCTErrorCodeFull = 13,               /* Insertion failed because database is full */
    WCTErrorCodeCantOpen = 14,           /* Unable to open the database file */
    WCTErrorCodeProtocol = 15,           /* Database lock protocol error */
    WCTErrorCodeEmpty = 16,              /* Database is empty */
    WCTErrorCodeSchema = 17,             /* The database schema changed */
    WCTErrorCodeExceed = 18,             /* String, BLOB, File, Concurrency exceeds size limit */
    WCTErrorCodeConstraint = 19,         /* Abort due to constraint violation */
    WCTErrorCodeMismatch = 20,           /* Data type mismatch */
    WCTErrorCodeMisuse = 21,             /* WCDB or SQLite used incorrectly */
    WCTErrorCodeNoLargeFileSupport = 22, /* Uses OS features not supported on host */
    WCTErrorCodeAuthorization = 23,      /* Authorization denied */
    WCTErrorCodeFormat = 24,             /* Auxiliary database format error */
    WCTErrorCodeRange = 25,              /* 2nd parameter to sqlite3_bind out of range or System call result too large */
    WCTErrorCodeNotADatabase = 26,       /* File opened that is not a database file */
    WCTErrorCodeNotice = 27,             /* Notifications */
    WCTErrorCodeWarning = 28,            /* Warnings */
};

@interface WCTError : NSError

@property(nonatomic, readonly) WCTErrorLevel level;

@property(nonatomic, readonly, nullable) NSString *message;

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

- (int)extendedCode;

@end

@interface WCTError (Source)

- (nullable NSString *)source;

@end

NS_ASSUME_NONNULL_END
