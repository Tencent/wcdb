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
#import <WCDB/WCTChainCall.h>
#import <WCDB/WCTDeclare.h>

/**
 Not Thread-safe
 */
@interface WCTInsert : WCTChainCall

/**
 @brief Execute the insert SQL with objects.
        Note that it will run embedded transaction while objects.count>1 .
        The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
 @param objects Objects to be inserted into WCDB.
 @return YES if no error occurs.
 */
- (BOOL)executeWithObjects:(NSArray<WCTObject *> *)objects;

@end
