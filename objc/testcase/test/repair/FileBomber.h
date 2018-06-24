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

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, FileBomberAttackType) {
    FileBomberAttackTypeZero,
    FileBomberAttackTypeRandom,
};

@interface FileBomber : NSObject

- (instancetype)initWithPath:(NSString *)path;

@property(nonatomic, assign) FileBomberAttackType attackType;

- (BOOL)attack:(NSRange)range;

- (BOOL)randomAttack;

@end

@interface DatabaseBomber : FileBomber

@property(nonatomic, assign) int pageSize;

- (BOOL)attackRootPage;

- (BOOL)attackPage:(int)pageno;

- (BOOL)randomAttackPage;

- (BOOL)attackWalHeader;

- (BOOL)attackWalFrame:(int)frameno;

- (BOOL)randomAttackWalFrame;

@end

NS_ASSUME_NONNULL_END
