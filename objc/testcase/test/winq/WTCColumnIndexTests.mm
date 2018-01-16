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

#import "WTCAssert.h"
#import "WTCBaseTestCase.h"

using namespace WCDB;

@interface WTCColumnIndexTests : WTCBaseTestCase

@end

@implementation WTCColumnIndexTests

- (void)testColumnIndex
{
    //Give
    Column column1 = Column("column1");
    Expression expression1 = column1.abs();
    
    //Then
    WINQAssertEqual(ColumnIndex(column1), @"column1");
    
    WINQAssertEqual(ColumnIndex(column1, OrderTerm::ASC), @"column1 ASC");
    
    WINQAssertEqual(ColumnIndex(expression1), @"ABS(column1)");
    
    WINQAssertEqual(ColumnIndex(expression1, OrderTerm::ASC), @"ABS(column1) ASC");
}

@end
