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

#import "TestCaseAssertion.h"
#import "TestCaseLog.h"
#import "TestCaseMacro.h"

#import "BaseTestCase.h"
#import "Benchmark.h"
#import "CRUDTestCase.h"
#import "DatabaseTestCase.h"
#import "TableTestCase.h"

#import "Dispatch.h"
#import "Random.h"
#import "Signpost.h"
#import "TestCaseCounter.h"
#import "TestCaseResult.h"

#import "Random+TestCaseObject.h"
#import "Random+WCDB.h"

#import "TestCaseObject+WCTTableCoding.h"
#import "TestCaseObject.h"

#import "AllTypesObject+WCTTableCoding.h"
#import "AllTypesObject.h"

#import "NSObject+TestCase.h"

#import <WCDB/WCTDatabase+Test.h>
