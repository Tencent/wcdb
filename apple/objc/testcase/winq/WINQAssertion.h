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

#import "TestCase.h"
#import <Foundation/Foundation.h>

namespace WCDB {

NSString *getDescriptionOfType(WCDB::Syntax::Identifier::Type type);
NSString *getDescriptionOfTypes(const std::list<WCDB::Syntax::Identifier::Type> &);
std::list<WCDB::Syntax::Identifier::Type> getTypesOfSQL(WCDB::SQL &sql);

}

#define TestCaseAssertIterateEqual(_sql, _expected)                                    \
    {                                                                                  \
        std::list<WCDB::Syntax::Identifier::Type> __types = WCDB::getTypesOfSQL(_sql); \
        std::list<WCDB::Syntax::Identifier::Type> __expected = (_expected);            \
        TestCaseAssertTypesEqual(__types, __expected);                                 \
    }

#define TestCaseAssertWINQConvertible(_Type, _parameter, _sql) TestCaseAssertSQLEqual([](const _Type &_value) -> _Type { return _value; }(_parameter), _sql)

#define TestCaseAssertTypesEqual(_left, _right) \
    TestCaseAssertStringEqual(WCDB::getDescriptionOfTypes(_left), WCDB::getDescriptionOfTypes(_right))
