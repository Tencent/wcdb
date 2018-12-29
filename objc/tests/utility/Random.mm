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

#import "Random.h"
#import <random>

@implementation Random {
    std::shared_ptr<std::default_random_engine> _engine;
    std::shared_ptr<std::uniform_int_distribution<uint64_t>> _uniformUInt64;
    std::shared_ptr<std::uniform_int_distribution<uint32_t>> _uniformUInt32;
    std::shared_ptr<std::uniform_int_distribution<uint8_t>> _uniformUInt8;
    std::shared_ptr<std::uniform_int_distribution<int64_t>> _uniformInt64;
    std::shared_ptr<std::uniform_int_distribution<int32_t>> _uniformInt32;
    std::shared_ptr<std::uniform_int_distribution<bool>> _uniformBool;
    std::shared_ptr<std::uniform_real_distribution<double>> _uniformDouble;
    std::shared_ptr<std::uniform_real_distribution<float>> _uniformFloat;
    std::shared_ptr<std::uniform_real_distribution<float>> _uniformFloat_0_1;
    std::shared_ptr<std::uniform_int_distribution<unsigned char>> _uniformUChar;
}

- (instancetype)init
{
    if (self = [super init]) {
        std::random_device rd;
        _engine.reset(new std::default_random_engine(rd()));
        _uniformUInt64.reset(new std::uniform_int_distribution<uint64_t>(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()));
        _uniformUInt32.reset(new std::uniform_int_distribution<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()));
        _uniformUInt8.reset(new std::uniform_int_distribution<uint8_t>(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()));
        _uniformInt64.reset(new std::uniform_int_distribution<int64_t>(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()));
        _uniformInt32.reset(new std::uniform_int_distribution<int32_t>(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()));
        _uniformBool.reset(new std::uniform_int_distribution<bool>(std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()));
        _uniformDouble.reset(new std::uniform_real_distribution<double>(std::numeric_limits<double>::min(), std::numeric_limits<double>::max()));
        _uniformFloat.reset(new std::uniform_real_distribution<float>(std::numeric_limits<float>::min(), std::numeric_limits<float>::max()));
        _uniformFloat_0_1.reset(new std::uniform_real_distribution<float>(0, 1));
        _uniformUChar.reset(new std::uniform_int_distribution<unsigned char>(std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max()));
    }
    return self;
}

- (void)setSeed:(uint32_t)seed
{
    _engine->seed(seed);
}

- (uint64_t)uint64
{
    return (*_uniformUInt64.get())(*_engine.get());
}

- (uint32_t)uint32
{
    return (*_uniformUInt32.get())(*_engine.get());
}

- (uint8_t)uint8
{
    return (*_uniformUInt8.get())(*_engine.get());
}

- (int64_t)int64
{
    return (*_uniformInt64.get())(*_engine.get());
}

- (int32_t)int32
{
    return (*_uniformInt32.get())(*_engine.get());
}

- (double)double_
{
    return (*_uniformDouble.get())(*_engine.get());
}

- (float)float_
{
    return (*_uniformFloat.get())(*_engine.get());
}

- (float)float_0_1
{
    return (*_uniformFloat_0_1.get())(*_engine.get());
}

- (BOOL)boolean
{
    return (*_uniformBool.get())(*_engine.get());
}

- (NSNumber *)number
{
    switch (self.uint8 % 3) {
    case 0:
        return [NSNumber numberWithInt:self.int32];
    case 1:
        return [NSNumber numberWithLongLong:self.uint64];
    case 2:
        return [NSNumber numberWithDouble:self.double_];
    }
    return nil;
}

- (int)length
{
    int length;
    do {
        length = self.uint8;
    } while (length < 8);
    return length;
}

- (NSString *)string
{
    static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    char result[std::numeric_limits<uint8_t>::max()];
    int length = self.length;
    for (int i = 0; i < length; ++i) {
        result[i] = alphanum[self.uint8 % (sizeof(alphanum) - 1)];
    }
    result[length] = '\0';
    return [NSString stringWithUTF8String:result];
}

- (NSData *)data
{
    return [self dataWithLength:self.length];
}

- (NSData *)dataWithLength:(NSInteger)length
{
    static_assert(sizeof(unsigned char) == 1, "");
    NSMutableData *data = [NSMutableData data];
    [data increaseLengthBy:length];
    unsigned char *bytes = (unsigned char *) data.mutableBytes;
    for (NSUInteger i = 0; i < length; ++i) {
        bytes[i] = (*_uniformUChar.get())(*_engine.get());
    }
    return [NSData dataWithData:data];
}

- (NSData *)dataOtherThan:(NSData *)other
{
    NSData *data;
    do {
        data = self.data;
    } while ([other isEqualToData:data]);
    return data;
}

@end
