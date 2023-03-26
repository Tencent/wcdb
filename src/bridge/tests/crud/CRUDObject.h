//
// Created by qiuwenchen on 2022/4/25.
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

#import <Foundation/Foundation.h>
#if TEST_WCDB_OBJC
#import <WCDBOBjc/WCTMacro.h>
#elif TEST_WCDB_CPP
#import <WCDBCpp/WCTMacro.h>
#else
#import <WCDB/WCTMacro.h>
#endif
#if TEST_WCDB_OBJC
#import <WCDBOBjc/WCTTableCoding.h>
#elif TEST_WCDB_CPP
#import <WCDBCpp/WCTTableCoding.h>
#else
#import <WCDB/WCTTableCoding.h>
#endif

@interface CRUDObject : NSObject <WCTTableCoding>

@property (nonatomic, assign) long variable1;
@property (nonatomic, strong) NSString* variable2;

WCDB_PROPERTY(variable1)
WCDB_PROPERTY(variable2)

@end
