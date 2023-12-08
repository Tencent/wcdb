//
// Created by qiuwenchen on 2023/12/3.
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

#import "Random+CompressionTestObject.h"
#import <Foundation/Foundation.h>

@implementation Random (Compression)

- (CompressionTestObject*)autoIncrementCompressionObject
{
    CompressionTestObject* obj = [[CompressionTestObject alloc] init];
    obj.isAutoIncrement = true;
    obj.subId = self.double_;
    obj.text = [self englishStringWithLength:self.uint16 % 1000];
    obj.textMatchId = self.int8 % 6;
    obj.blob = [[self englishStringWithLength:self.uint16 % 1000] dataUsingEncoding:NSUTF8StringEncoding];
    obj.blobMatchId = self.int8 % 4;
    return obj;
}

- (CompressionTestObject*)compressionObjectWithId:(int)identifier
{
    CompressionTestObject* obj = [[CompressionTestObject alloc] init];
    obj.mainId = identifier;
    obj.subId = self.double_;
    obj.text = [self englishStringWithLength:self.uint16 % 1000];
    obj.textMatchId = self.int8 % 6;
    obj.blob = [[self englishStringWithLength:self.uint16 % 1000] dataUsingEncoding:NSUTF8StringEncoding];
    obj.blobMatchId = self.int8 % 4;
    return obj;
}

- (NSArray<CompressionTestObject*>*)autoIncrementCompressionObjectWithCount:(int)count
{
    NSMutableArray* ret = [[NSMutableArray alloc] init];
    for (int i = 0; i < count; i++) {
        [ret addObject:[self autoIncrementCompressionObject]];
    }
    return ret;
}

- (NSArray<NSString*>*)compressionStringWithCount:(int)count
{
    NSMutableArray<NSString*>* ret = [[NSMutableArray alloc] init];
    for (int i = 0; i < count; i++) {
        [ret addObject:self.englishString];
    }
    return ret;
}

@end
