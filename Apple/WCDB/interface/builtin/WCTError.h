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
#import <type_traits>
#import <WCDB/error.hpp>

/**
 It indicates the error type for WCTError
 
 - WCTErrorTypeSQLite: an error comes from sqlite interface. You can check it at http://www.sqlite.org/rescode.html .
 - WCTErrorTypeSystemCall: system error, which is obtained by [errno].
 - WCTErrorTypeCore: an error comes from core layer of WCDB. For further information, see [Error::CoreCode] in [error.hpp].
 - WCTErrorTypeInterface: an error comes from interface layer of WCDB. For further information, see [Error::InterfaceCode] in [error.hpp].
 - WCTErrorTypeAbort: development error. You should fix it before release.
 - WCTErrorTypeWarning: warning. You'd better fix it,
 - WCTErrorTypeSQLiteGlobal: an error comes from [SQLITE_CONFIG_LOG]. You can check it at http://www.sqlite.org/rescode.html .
 */
typedef NS_ENUM(std::underlying_type<WCDB::Error::Type>::type, WCTErrorType) {
    WCTErrorTypeNotSet = (WCTErrorType)WCDB::Error::Type::NotSet,
    WCTErrorTypeSQLite = (WCTErrorType)WCDB::Error::Type::SQLite,
    WCTErrorTypeSystemCall = (WCTErrorType)WCDB::Error::Type::SystemCall,
    WCTErrorTypeCore = (WCTErrorType)WCDB::Error::Type::Core,
    WCTErrorTypeInterface = (WCTErrorType)WCDB::Error::Type::Interface,
    WCTErrorTypeAbort = (WCTErrorType)WCDB::Error::Type::Abort,
    WCTErrorTypeWarning = (WCTErrorType)WCDB::Error::Type::Warning,
    WCTErrorTypeSQLiteGlobal = (WCTErrorType)WCDB::Error::Type::SQLiteGlobal,
};

/**
 More information can be obtained with following keys.
 Note that different types of errors have different keys.

 - WCTErrorKeyTag: tag of database.
 - WCTErrorKeyOperation: for further information, see [Error::CoreOperation] in [error.hpp].
 - WCTErrorKeyExtendedCode: extended code for sqlite. You can check it at http://www.sqlite.org/rescode.html .
 - WCTErrorKeyMessage: some text message that helps you debug it.
 - WCTErrorKeySQL: the SQL is being executed when an error occurs.
 - WCTErrorKeyPath: the file path that is being manipulated when an error occurs.
 */
typedef NS_ENUM(std::underlying_type<WCDB::Error::Key>::type, WCTErrorKey) {
    WCTErrorKeyTag = (WCTErrorKey)WCDB::Error::Key::Tag,
    WCTErrorKeyOperation = (WCTErrorKey)WCDB::Error::Key::Operation,
    WCTErrorKeyExtendedCode = (WCTErrorKey)WCDB::Error::Key::ExtendedCode,
    WCTErrorKeyMessage = (WCTErrorKey)WCDB::Error::Key::Message,
    WCTErrorKeySQL = (WCTErrorKey)WCDB::Error::Key::SQL,
    WCTErrorKeyPath = (WCTErrorKey)WCDB::Error::Key::Path,
};

@interface WCTError : NSError

/**
 Convenient interface for checking code==0.
 Note that both error==nil and error.code==0 mean no error.
 
 @return true for no error 
 */
- (BOOL)isOK;

/**
 See [WCTErrorType] in [WCTError.h] also.
 */
@property(nonatomic, readonly) WCTErrorType type;

/**
 See [WCTErrorKey] in [WCTError.h> also.

 @param key 
 @return Depending on the key and type, [NSString], [NSNumber] and [nil] may be returned.
 */
- (id)infoForKey:(WCTErrorKey)key;

@end
