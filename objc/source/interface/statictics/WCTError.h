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
    WCTErrorLevelError = 4,
    WCTErrorLevelFatal = 5,
};

typedef NS_ENUM(NSUInteger, WCTErrorOperation) {
    WCTErrorOperationNotSet = 0,
    WCTErrorOperationFileLstat = 1,
    WCTErrorOperationFileAccess = 2,
    WCTErrorOperationFileLink = 3,
    WCTErrorOperationFileUnlink = 4,
    WCTErrorOperationFileRemove = 5,
    WCTErrorOperationFileMkdir = 6,
    WCTErrorOperationFileGetAttribute = (1 | (1 << 8)),
    WCTErrorOperationFileSetAttribute = (2 | (1 << 8)),
};

typedef NS_ENUM(NSUInteger, WCTErrorType) {
    WCTErrorTypeSQLite = 1,
    WCTErrorTypeHandle = 2,
    WCTErrorTypeCore = 3,
    WCTErrorTypeFile = 4,
};

typedef NSString *WCTErrorKey;
WCDB_EXTERN WCTErrorKey const WCTErrorKeyPath;
WCDB_EXTERN WCTErrorKey const WCTErrorKeySQL;
WCDB_EXTERN WCTErrorKey const WCTErrorKeyOperation;
WCDB_EXTERN WCTErrorKey const WCTErrorKeyTag;
WCDB_EXTERN WCTErrorKey const WCTErrorKeyExtendedCode;

typedef int32_t WCTTag;
WCDB_EXTERN WCTTag const WCTInvalidTag;

/**
 Detailed error
 */
@interface WCTError : NSError

@property(nonatomic, readonly) WCTErrorLevel level;

@property(nonatomic, readonly) WCTErrorType type;

@property(nonatomic, readonly, nullable) NSString *message;

- (nullable NSString *)stringForKey:(WCTErrorKey)key;

- (NSInteger)integerForKey:(WCTErrorKey)key;

@end

@interface WCTError (Operation)

- (WCTErrorOperation)operation;

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

- (NSInteger)extendedCode;

@end

NS_ASSUME_NONNULL_END
