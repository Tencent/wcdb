//
// Created by qiuwenchen on 2023/9/15.
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

#import "RepairTestObjectBase.h"
#import "NSObject+TestCase.h"
#import <Foundation/Foundation.h>

@implementation RepairTestObjectBase

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    RepairTestObjectBase* other = (RepairTestObjectBase*) object;
    if (self.identifier != other.identifier) {
        return NO;
    }
    if (self.doubleValue != other.doubleValue) {
        return NO;
    }
    if (![NSObject isObject:self.textValue nilEqualToObject:other.textValue]) {
        return NO;
    }
    if (![NSObject isObject:self.blobValue nilEqualToObject:other.blobValue]) {
        return NO;
    }
    return YES;
}

- (NSUInteger)hash
{
    NSMutableData* data = [NSMutableData data];
    [data appendBytes:&(_identifier) length:sizeof(_identifier)];
    [data appendBytes:&(_doubleValue) length:sizeof(_doubleValue)];
    [data appendData:[_textValue dataUsingEncoding:NSUTF8StringEncoding]];
    [data appendData:_blobValue];
    return data.hash;
}

@end
