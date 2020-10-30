//
// Created by sanhuazhang on 2019/07/05
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

#import <Foundation/Foundation.h>

@class WCTDatabase;

@protocol PrototypePreparation <NSObject>
@required
- (void)preparePrototype:(WCTDatabase*)prototype currentQuality:(double)quality;
- (double)qualityOfPrototype:(WCTDatabase*)prototype;
- (NSString*)categoryOfPrototype;
@optional
- (void)configurePrototype:(WCTDatabase*)prototype;

@end

@interface PrototypeFactory : NSObject

- (instancetype)init UNAVAILABLE_ATTRIBUTE;

- (instancetype)initWithDirectory:(NSString*)directory;

@property (nonatomic, readonly) WCTDatabase* database;

@property (nonatomic, readonly) NSString* directory;

@property (nonatomic, weak) id<PrototypePreparation> delegate;

@property (nonatomic, assign) double tolerance;

@property (nonatomic, assign) double quality;

- (void)produce:(NSString*)destination;

- (void)removePrototype;

@end
