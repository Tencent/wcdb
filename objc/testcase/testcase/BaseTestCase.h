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

#import <XCTest/XCTest.h>

@class Random;
@class Signpost;

@interface BaseTestCase : XCTestCase

@property (class, nonatomic, readonly) NSString* root;
@property (nonatomic, readonly) NSString* directory;

@property (nonatomic, readonly) NSString* className;
@property (nonatomic, readonly) NSString* testName;

@property (nonatomic, readonly) NSFileManager* fileManager;
@property (nonatomic, readonly) Random* random;
@property (nonatomic, readonly) Signpost* signpost;

- (void)log:(NSString*)format, ...;

- (void)doMeasure:(void (^)(void))block
            setUp:(void (^)(void))setUpBlock
         tearDown:(void (^)(void))tearDownBlock
 checkCorrectness:(void (^)(void))correctnessBlock;

@end

#define TestCaseSignpostBegin(task) \
    SignpostBegin(self.signpost, task)

#define TestCaseSignpostEnd(task) \
    SignpostEnd(self.signpost, task)

#define TestCaseFunctionSignpostBegin() \
    TestCaseSignpostBegin(__PRETTY_FUNCTION__)

#define TestCaseFunctionSignpostEnd() \
    TestCaseSignpostEnd(__PRETTY_FUNCTION__)
