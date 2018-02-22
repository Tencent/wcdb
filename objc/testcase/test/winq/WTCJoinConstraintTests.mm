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

#import "WTCWINQTestCase.h"

@interface WTCJoinConstraintTests : WTCWINQTestCase

@end

@implementation WTCJoinConstraintTests

- (void)testJoinConstraint
{
    WINQAssertEqual(WCDB::JoinConstraint()
                        .on(self.class.condition),
                    @"ON testColumn NOTNULL");

    WINQAssertEqual(WCDB::JoinConstraint()
                        .usingColumn(self.class.column),
                    @"USING(testColumn)");

    WINQAssertEqual(WCDB::JoinConstraint()
                        .usingColumns(self.class.columns),
                    @"USING(testColumn, testColumn2)");

    WINQAssertEqual(WCDB::JoinConstraint(), @"");
}

@end
