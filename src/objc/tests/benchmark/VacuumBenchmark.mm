//
// Created by qiuwenchen on 2023/12/23.
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

#import "SizeBasedFactory.h"
#import "TestCase.h"
#import <Foundation/Foundation.h>

@interface VacuumBenchmark : Benchmark

@end

@implementation VacuumBenchmark {
    SizeBasedFactory* _factory;
    NSData* _cipherKey;
}

- (SizeBasedFactory*)factory
{
    @synchronized(self) {
        if (_factory == nil) {
            _factory = [[SizeBasedFactory alloc] initWithDirectory:self.class.cacheRoot];
        }
        return _factory;
    }
}

- (NSData*)cipherKey
{
    @synchronized(self) {
        if (_cipherKey.length == 0) {
            _cipherKey = Random.shared.data;
        }
        return _cipherKey;
    }
}

- (void)setUp
{
    [super setUp];

    self.factory.needCipher = NO;
    [self.database setCipherKey:nil];
    self.factory.quality = 100 * 1024 * 1024;
    self.factory.tolerance = 0.02;
}

- (void)setUpDatabase
{
    TestCaseAssertTrue([self.database removeFiles]);
    [self.factory produce:self.path];
}

- (void)tearDownDatabase
{
    TestCaseAssertTrue([self.database removeFiles]);
}

- (void)test_vacuum
{
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database vacuum:nil];
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

- (void)test_cipher_vacuum
{
    __block BOOL result;
    [self.database setCipherKey:SizeBasedFactory.commonCipherKey];
    self.factory.needCipher = YES;
    [self
    doMeasure:^{
        result = [self.database vacuum:nil];
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

- (void)test_normal_vacuum
{
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database execute:WCDB::StatementVacuum().vacuum()];
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

- (void)test_normal_cipher_vacuum
{
    __block BOOL result;
    [self.database setCipherKey:SizeBasedFactory.commonCipherKey];
    self.factory.needCipher = YES;
    [self
    doMeasure:^{
        result = [self.database execute:WCDB::StatementVacuum().vacuum()];
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
