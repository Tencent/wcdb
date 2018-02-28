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
#import <WCDB/Error.hpp>
#import <type_traits>

/**
 It indicates the error type for WCTError. You can see Error::report method in the source code as a practical handling way.
 */
typedef NS_ENUM(int, WCTErrorType) {
    /**
     * An error comes from sqlite interface. You can check it at http://www.sqlite.org/rescode.html .
     */
    WCTErrorTypeSQLite = (WCTErrorType) WCDB::Error::Type::SQLite,
    /**
     * System error, which is obtained by [errno].
     */
    WCTErrorTypeSystemCall = (WCTErrorType) WCDB::Error::Type::SystemCall,
    /**
     * An error comes from core layer of WCDB. For further information, see [Error::CoreCode] in [Error.hpp].
     */
    WCTErrorTypeCore = (WCTErrorType) WCDB::Error::Type::Core,
    /**
     * An error comes from interface layer of WCDB. For further information, see [Error::InterfaceCode] in [Error.hpp].
     */
    WCTErrorTypeInterface = (WCTErrorType) WCDB::Error::Type::Interface,
    /**
     * Development error. You should fix it before release.
     */
    WCTErrorTypeAbort = (WCTErrorType) WCDB::Error::Type::Abort,
    /**
     * Warning. You'd better fix it.
     */
    WCTErrorTypeWarning = (WCTErrorType) WCDB::Error::Type::Warning,
    /**
     * An error comes from [SQLITE_CONFIG_LOG]. You can check it at http://www.sqlite.org/rescode.html .
     */
    WCTErrorTypeSQLiteGlobal = (WCTErrorType) WCDB::Error::Type::SQLiteGlobal,
};

/**
 Detailed error
 */
@interface WCTError : NSError

/**
 @brief init
 @param type type
 @param code code
 @param userInfo WCTErrorKey->Value
 @return self
 */
- (instancetype)initWithType:(WCTErrorType)type code:(NSInteger)code userInfo:(NSDictionary *)userInfo;

@property(nonatomic, readonly) WCTErrorType type;

- (NSNumber *)tag;

- (NSNumber *)operation;

- (NSNumber *)extendedCode;

- (NSString *)message;

- (NSString *)sql;

- (NSString *)path;

/**
 @brief Convenient interface for checking code==0.
 @return YES for no error 
 */
- (BOOL)isOK;

@end
