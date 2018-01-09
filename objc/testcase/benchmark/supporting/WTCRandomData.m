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

#import "WTCRandomData.h"

@implementation WTCRandomData {
    NSData *_data;
    NSUInteger _pos;
}

- (instancetype)initWithSeed:(unsigned int)seed
{
    if (self = [super init]) {
        const NSUInteger length = 1024 * 1024;
        NSMutableData *data = [NSMutableData dataWithCapacity:length];
        srand(seed);
        while (data.length < length) {
            uint32_t value = (uint32_t) random();
            [data appendBytes:&value length:sizeof(value)];
        }
        _data = [NSData dataWithData:data];
        _pos = 0;
    }
    return self;
}

- (NSData *)dataWithLength:(NSUInteger)length
{
    if (_pos + length > _data.length) {
        _pos = 0;
    }
    NSData *data = [_data subdataWithRange:NSMakeRange(_pos, length)];
    _pos += length;
    return data;
}

@end
