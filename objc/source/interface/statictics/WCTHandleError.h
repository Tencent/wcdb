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

#import <WCDB/WCTError.h>

typedef NS_ENUM(NSUInteger, WCTHandleOperation) {
    WCTHandleOperationNotSet = 0,
    WCTHandleOperationPrepare = 1,
    WCTHandleOperationExecute = 2,
    WCTHandleOperationOpen = 3,
    WCTHandleOperationStep = 4,
    WCTHandleOperationCipher = 5,
    WCTHandleOperationRepair = 6,
    WCTHandleOperationBackup = 7,
};

@interface WCTHandleError : WCTError

@property(nonatomic, readonly) WCTTag tag;

@property(nonatomic, readonly) int extendedCode;

@property(nonatomic, readonly) NSString *path;

@property(nonatomic, readonly) WCDB::Statement statement;

@property(nonatomic, readonly) WCTHandleOperation operation;

@end
