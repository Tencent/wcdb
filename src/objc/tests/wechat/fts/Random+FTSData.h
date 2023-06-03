//
// Created by qiuwenchen on 2020/8/30.
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

#import "FTSData.h"
#import "Random.h"

NS_ASSUME_NONNULL_BEGIN

@interface Random (FTSData)

- (NSArray<FTS3MsgContentItem*>*)randomFTS3Items:(NSInteger)count;

- (NSArray<FTS5MsgContentItem*>*)randomFTS5Items:(NSInteger)count;

- (NSArray<FTS5NewFavSearchItem*>*)randomFavFTSItem:(NSInteger)count;

- (NSArray<FTS5ContactSearchItem*>*)randomContactFTSItem:(NSInteger)count;

@end

NS_ASSUME_NONNULL_END
