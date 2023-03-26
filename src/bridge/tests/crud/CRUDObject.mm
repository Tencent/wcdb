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

#import "CRUDObject.h"
#if TEST_WCDB_OBJC
#import <WCDBOBjc/WCDB.h>
#elif TEST_WCDB_CPP
#import <WCDBCpp/WCDB.h>
#else
#import <WCDB/WCDB.h>
#endif

@implementation CRUDObject
- (BOOL)isEqual:(CRUDObject*)object
{
    return _variable1 == object.variable1 && ((_variable2 == nil && object.variable2 == nil) || [_variable2 isEqualToString:object.variable2]);
}

@synthesize lastInsertedRowID;

WCDB_IMPLEMENTATION(CRUDObject)
WCDB_SYNTHESIZE(variable1)
WCDB_SYNTHESIZE(variable2)

WCDB_PRIMARY_ASC_AUTO_INCREMENT(variable1)

@end
