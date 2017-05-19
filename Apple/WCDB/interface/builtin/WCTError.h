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

typedef NS_ENUM(std::underlying_type<WCDB::Error::Key>::type, WCTErrorKey) {
    WCTErrorKeyTag = (WCTErrorKey)WCDB::Error::Key::Tag,
    WCTErrorKeyOperation = (WCTErrorKey)WCDB::Error::Key::Operation,
    WCTErrorKeyExtendedCode = (WCTErrorKey)WCDB::Error::Key::ExtendedCode,
    WCTErrorKeyMessage = (WCTErrorKey)WCDB::Error::Key::Message,
    WCTErrorKeySQL = (WCTErrorKey)WCDB::Error::Key::SQL,
    WCTErrorKeyPath = (WCTErrorKey)WCDB::Error::Key::Path,
};

@interface WCTError : NSError

- (BOOL)isOK;

@property(nonatomic, readonly) WCTErrorType type;

- (id)infoForKey:(WCTErrorKey)key;

@end
