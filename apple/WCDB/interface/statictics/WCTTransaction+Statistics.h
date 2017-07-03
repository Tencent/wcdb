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
#import <WCDB/WCTTransaction.h>

@interface WCTTransaction (Statistics)

/**
 More detailed error messages.
 
 @return nil or error.isOK==YES if no error occurs.
 */
- (WCTError *)error;

/**
 After enabling, you can call the [cost] interface to get time consuming.
 Note that you should call it before calling all other interface, otherwise [cost] will return an inaccurate result.
 See [cost] also.
 
 @param enabled enabled
 */
- (void)setStatisticsEnabled:(BOOL)enabled;

/**
 The time consuming. You can call it to profile the performance.
 See [setStatisticsEnabled:] also.
 
 @return Time in seconds
 */
- (double)cost;

@end
