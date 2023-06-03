//
// Created by qiuwenchen on 2022/8/14.
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

#import "CPPTableTestCase.h"

@interface CPPCRUDTestCase : CPPTableTestCase

#pragma mark - object
@property (nonatomic, readonly) CPPTestCaseObject& object1;
@property (nonatomic, readonly) CPPTestCaseObject& object2;
@property (nonatomic, readonly) WCDB::ValueArray<CPPTestCaseObject>& objects;
- (void)insertPresetObjects;

#pragma mark - value
@property (nonatomic, readonly) WCDB::OneRowValue row1;
@property (nonatomic, readonly) WCDB::OneRowValue row2;
@property (nonatomic, readonly) WCDB::MultiRowsValue rows;

- (void)insertPresetRows;
- (int64_t)nextIdentifier;
- (int)allRowsCount;
@end
