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

#import "TablesBasedFactory.h"
#import "TestCase.h"

@interface TableBenchmark : Benchmark
@property (nonatomic, readonly) TablesBasedFactory* factory;
@end

@implementation TableBenchmark {
    TablesBasedFactory* _factory;
}

- (TablesBasedFactory*)factory
{
    @synchronized(self) {
        if (_factory == nil) {
            _factory = [[TablesBasedFactory alloc] initWithDirectory:self.class.cacheRoot];
        }
        return _factory;
    }
}

- (void)setUp
{
    [super setUp];

    self.factory.tolerance = 0.0f;
    self.factory.quality = 100000;
}

- (void)setUpDatabase
{
    TestCaseAssertTrue([self.database removeFiles]);
    [self.factory produce:self.path];

    [self.database close];
    TestCaseAssertTrue([self.database canOpen]);
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
}

- (void)test_initialization
{
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database canOpen];
    }
    setUp:^{
        [self setUpDatabase];
        [self.database close]; // reset cache
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)test_exists
{
    __block NSArray<NSString*>* tableNames;
    __block BOOL result;
    [self
    doMeasure:^{
        result = YES;
        for (NSString* tableName in tableNames) {
            if (![self.database tableExists:tableName]) {
                result = NO;
                break;
            }
        }
    }
    setUp:^{
        [self setUpDatabase];
        if (tableNames == nil) {
            NSString* pattern = [NSString stringWithFormat:@"%s%%", WCDB::Syntax::builtinTablePrefix];

            NSMutableArray<NSString*>* names = [NSMutableArray arrayWithCapacity:(NSUInteger) self.factory.quality];
            for (WCTValue* value in [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCTMaster.name).from(WCTMaster.tableName).where(WCTMaster.name.notLike(pattern))]) {
                [names addObject:value.stringValue];
            }
            tableNames = [NSArray arrayWithArray:names];
        }
        TestCaseAssertTrue(tableNames.count == self.factory.quality);
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)test_create_tables
{
    int numberOfTables = 100;
    __block NSArray* tableNames = nil;
    __block BOOL result;
    [self
    doMeasure:^{
        for (NSString* tableName in tableNames) {
            if (![self.database createTable:tableName withClass:TestCaseObject.class]) {
                result = NO;
                return;
            }
        }
        result = YES;
    }
    setUp:^{
        [self setUpDatabase];

        if (tableNames == nil) {
            tableNames = [NSArray arrayWithArray:[Random.shared tableNamesWithCount:numberOfTables]];
        }
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
