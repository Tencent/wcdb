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
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTInterface.h>
#import <WCDB/abstract.h>

@interface WCTInterface (Core)

/**
 @brief Exec a specific sql.
        Note that you can use this interface to execute a SQL that is not contained in the WCDB interface layer 
 @param statement WINQ statement
 @return YES if it is successfully executed
 */
- (BOOL)exec:(const WCDB::Statement &)statement;

/**
 @brief This interface is equivalent to [interface prepare:statement withError:nil];
 @param statement WINQ statement
 @return WCTStatement
 @see WCTStatement
 */
- (WCTStatement *)prepare:(const WCDB::Statement &)statement;

/**
 @brief Prepare a specific sql.
 Note that you can use this interface to prepare a SQL that is not contained in the WCDB interface layer
 @param statement WINQ statement
 @return WCTStatement
 @see WCTStatement
 */
- (WCTStatement *)prepare:(const WCDB::Statement &)statement withError:(WCTError **)error;

@end
