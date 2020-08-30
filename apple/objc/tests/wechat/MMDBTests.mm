//
// Created by sanhuazhang on 2019/12/9.
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

#import "MMDBTests.h"

@implementation MMDBTests {
    MMDBFactory* _factory;
}

- (MMDBFactory*)factory
{
    @synchronized(self) {
        if (_factory == nil) {
            _factory = [[MMDBFactory alloc] initWithDirectory:self.class.cacheRoot];
        }
        return _factory;
    }
}

- (void)setUp
{
    [super setUp];

    self.factory.tolerance = 0.0f;
    self.factory.quality = 1000000;
}

- (void)setUpDatabase
{
    [self.factory produce:self.path];
    self.tableName = self.factory.tableName;

    TestCaseAssertOptionalEqual([self.database getNumberOfWalFrames], 0);
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
}

- (void)doTestGetMaxTime
{
    [self
    doMeasure:^{
        [self.database getValueOnResultColumn:DBMessage.createTime.max() fromTable:self.tableName];
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
    }
    checkCorrectness:nil];
}

#pragma mark - Tests
- (void)test_get_max_time
{
    [self doTestGetMaxTime];
}

@end
