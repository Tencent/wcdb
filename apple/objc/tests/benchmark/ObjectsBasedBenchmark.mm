//
// Created by sanhuazhang on 2019/05/02
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

#import "ObjectsBasedBenchmark.h"

@implementation ObjectsBasedBenchmark {
    ObjectsBasedFactory* _factory;
}

- (ObjectsBasedFactory*)factory
{
    @synchronized(self) {
        if (_factory == nil) {
            _factory = [[ObjectsBasedFactory alloc] initWithDirectory:self.class.cacheRoot];
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

- (void)doTestWrite
{
    int numberOfObjects = 10000;

    NSArray* objects = [Random.shared testCaseObjectsWithCount:numberOfObjects startingFromIdentifier:(int) self.factory.quality];
    __block BOOL result;
    [self
    doMeasure:^{
        for (TestCaseObject* object in objects) {
            if (![self.database insertObject:object intoTable:self.tableName]) {
                result = NO;
                return;
            }
        }
        result = YES;
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)doTestRead
{
    __block NSArray<TestCaseObject*>* result;
    [self
    doMeasure:^{
        result = [self.database getObjectsOfClass:TestCaseObject.class fromTable:self.tableName];
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
        result = nil;
    }
    checkCorrectness:^{
        TestCaseAssertEqual(result.count, self.factory.quality);
    }];
}

- (void)doTestBatchWrite
{
    int numberOfObjects = 1000000;

    NSArray* objects = [Random.shared testCaseObjectsWithCount:numberOfObjects startingFromIdentifier:(int) self.factory.quality];
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database insertObjects:objects intoTable:self.tableName];
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

@end
