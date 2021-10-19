//
// Created by 陈秋文 on 2020/8/30.
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

#import "PrototypeFactory.h"
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, FTSDataType) {
    FTSDataType_FTS3 = 0,
    FTSDataType_FTS5,
    FTSDataType_FTS5_RowidIndex,
    FTSDataType_Fav,
};

@interface FTSFactory : PrototypeFactory <PrototypePreparation>

@property (nonatomic, readonly) NSString* assistTableName;
@property (nonatomic, assign) BOOL needOptimize;
@property (nonatomic, assign) int tableCount;
@property (nonatomic, assign) FTSDataType dataType;
@property (nonatomic, assign) int autoMergeCount;
@property (nonatomic, assign) NSString* tokenizerName;

- (NSString*)indexTableNameOf:(int)tableId;

@end

NS_ASSUME_NONNULL_END
