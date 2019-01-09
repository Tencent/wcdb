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
#import <os/signpost.h>

@interface Signpost : NSObject

- (instancetype)initWithSystem:(NSString*)system andCategory:(NSString*)category;
@property (nonatomic, readonly) NSString* system;
@property (nonatomic, readonly) NSString* category;

@property (nonatomic, readonly) os_log_t log;
@property (nonatomic, readonly) os_signpost_id_t identifier;

@end

#define SignpostBegin(signpost, task) \
    os_signpost_interval_begin(signpost.log, signpost.identifier, task)

#define SignpostEnd(signpost, task) \
    os_signpost_interval_end(signpost.log, signpost.identifier, task)
