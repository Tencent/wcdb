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

#import <Foundation/Foundation.h>

//TODO make this all-typed object to the default test case object.
//TODO add array/dictionary/date and other builtin type
@interface RepairTestCaseObject : NSObject

+ (RepairTestCaseObject *)randomObject;
+ (RepairTestCaseObject *)maxObject;
+ (RepairTestCaseObject *)minObject;
+ (RepairTestCaseObject *)zeroObject;
+ (NSArray<RepairTestCaseObject *> *)randomObjects;
+ (NSArray<RepairTestCaseObject *> *)randomObjects:(int)count;

- (BOOL)isEqualToRepairTestCaseObject:(RepairTestCaseObject *)object;

@property(nonatomic, assign) BOOL boolValue;
@property(nonatomic, assign) int8_t int8Value;
@property(nonatomic, assign) int16_t int16Value;
@property(nonatomic, assign) int32_t int32Value;
@property(nonatomic, assign) int64_t int64Value;
@property(nonatomic, assign) double doubleValue;
@property(nonatomic, retain) NSString *stringValue;
@property(nonatomic, retain) NSData *dataValue;
@property(nonatomic, retain) NSObject *nullValue;

@end
