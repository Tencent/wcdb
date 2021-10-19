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

#import "Random.h"
#import <random>

@implementation Random {
    std::shared_ptr<std::default_random_engine> _unstableEngine;
    std::shared_ptr<std::default_random_engine> _stableEngine;
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
    std::shared_ptr<std::uniform_int_distribution<int>> _uniformLength;
    BOOL _stable;
}

+ (instancetype)shared
{
    static Random *s_random = [[Random alloc] init];
    return s_random;
}

- (void)setStable:(BOOL)stable
{
    @synchronized(self) {
        _stable = stable;
    }
}

- (void)reset
{
    _stableEngine.reset();
    _unstableEngine.reset();
    _uniformUInt64.reset();
    _uniformUInt32.reset();
    _uniformUInt8.reset();
    _uniformInt64.reset();
    _uniformInt32.reset();
    _uniformBool.reset();
    _uniformDouble.reset();
    _uniformFloat.reset();
    _uniformFloat_0_1.reset();
    _uniformUChar.reset();
    _uniformUChar.reset();
    _uniformLength.reset();
}

- (std::shared_ptr<std::default_random_engine> &)engine
{
    @synchronized(self) {
        if (_stable) {
            if (_stableEngine == nullptr) {
                _stableEngine = std::make_shared<std::default_random_engine>(0);
            }
            return _stableEngine;
        } else {
            if (_unstableEngine == nullptr) {
                std::random_device rd;
                _unstableEngine = std::make_shared<std::default_random_engine>(rd());
            }
            return _unstableEngine;
        }
    }
}
- (std::shared_ptr<std::uniform_int_distribution<uint64_t>> &)uniformUInt64
{
    @synchronized(self) {
        if (_uniformUInt64 == nullptr) {
            _uniformUInt64 = std::make_shared<std::uniform_int_distribution<uint64_t>>(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
        }
        return _uniformUInt64;
    }
}
- (std::shared_ptr<std::uniform_int_distribution<uint32_t>> &)uniformUInt32
{
    @synchronized(self) {
        if (_uniformUInt32 == nullptr) {
            _uniformUInt32 = std::make_shared<std::uniform_int_distribution<uint32_t>>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
        }
        return _uniformUInt32;
    }
}
- (std::shared_ptr<std::uniform_int_distribution<uint8_t>> &)uniformUInt8
{
    @synchronized(self) {
        if (_uniformUInt8 == nullptr) {
            _uniformUInt8 = std::make_shared<std::uniform_int_distribution<uint8_t>>(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
        }
        return _uniformUInt8;
    }
}
- (std::shared_ptr<std::uniform_int_distribution<int64_t>> &)uniformInt64
{
    @synchronized(self) {
        if (_uniformInt64 == nullptr) {
            _uniformInt64 = std::make_shared<std::uniform_int_distribution<int64_t>>(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());
        }
        return _uniformInt64;
    }
}
- (std::shared_ptr<std::uniform_int_distribution<int32_t>> &)uniformInt32
{
    @synchronized(self) {
        if (_uniformInt32 == nullptr) {
            _uniformInt32 = std::make_shared<std::uniform_int_distribution<int32_t>>(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());
        }
        return _uniformInt32;
    }
}
- (std::shared_ptr<std::uniform_int_distribution<bool>> &)uniformBool
{
    @synchronized(self) {
        if (_uniformBool == nullptr) {
            _uniformBool = std::make_shared<std::uniform_int_distribution<bool>>(std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max());
        }
        return _uniformBool;
    }
}
- (std::shared_ptr<std::uniform_real_distribution<double>> &)uniformDouble
{
    @synchronized(self) {
        if (_uniformDouble == nullptr) {
            _uniformDouble = std::make_shared<std::uniform_real_distribution<double>>(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
        }
        return _uniformDouble;
    }
}
- (std::shared_ptr<std::uniform_real_distribution<float>> &)uniformFloat
{
    @synchronized(self) {
        if (_uniformFloat == nullptr) {
            _uniformFloat = std::make_shared<std::uniform_real_distribution<float>>(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
        }
        return _uniformFloat;
    }
}
- (std::shared_ptr<std::uniform_real_distribution<float>> &)uniformFloat_0_1
{
    @synchronized(self) {
        if (_uniformFloat_0_1 == nullptr) {
            _uniformFloat_0_1 = std::make_shared<std::uniform_real_distribution<float>>(0, 1);
        }
        return _uniformFloat_0_1;
    }
}
- (std::shared_ptr<std::uniform_int_distribution<unsigned char>> &)uniformUChar
{
    @synchronized(self) {
        if (_uniformUChar == nullptr) {
            _uniformUChar = std::make_shared<std::uniform_int_distribution<unsigned char>>(std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max());
        }
        return _uniformUChar;
    }
}

- (std::shared_ptr<std::uniform_int_distribution<int>> &)uniformLength
{
    @synchronized(self) {
        if (_uniformLength == nullptr) {
            if (_stable) {
                _uniformLength = std::make_shared<std::uniform_int_distribution<int>>(100, 100);
            } else {
                _uniformLength = std::make_shared<std::uniform_int_distribution<int>>(1, 100);
            }
        }
        return _uniformLength;
    }
}

- (uint64_t)uint64
{
    return (*self.uniformUInt64.get())(*self.engine.get());
}

- (uint32_t)uint32
{
    return (*self.uniformUInt32.get())(*self.engine.get());
}

- (uint8_t)uint8
{
    return (*self.uniformUInt8.get())(*self.engine.get());
}

- (int64_t)int64
{
    return (*self.uniformInt64.get())(*self.engine.get());
}

- (int32_t)int32
{
    return (*self.uniformInt32.get())(*self.engine.get());
}

- (double)double_
{
    return (*self.uniformDouble.get())(*self.engine.get());
}

- (float)float_
{
    return (*self.uniformFloat.get())(*self.engine.get());
}

- (float)float_0_1
{
    return (*self.uniformFloat_0_1.get())(*self.engine.get());
}

- (BOOL)boolean
{
    return (*self.uniformBool.get())(*self.engine.get());
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
    return (*self.uniformLength.get())(*self.engine.get());
}

- (NSString *)string
{
    static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    char result[std::numeric_limits<uint8_t>::max() + 1];
    int length = self.length;
    for (int i = 0; i < length; ++i) {
        result[i] = alphanum[self.uint8 % (sizeof(alphanum) - 1)];
    }
    result[length] = '\0';
    return [NSString stringWithUTF8String:result];
}

- (NSString *)chineseString
{
    NSStringEncoding gbkEncoding = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
    NSMutableString *string = [NSMutableString string];
    for (int i = 0; i < self.length; i++) {
        UInt8 randomH = 0xA1 + self.uint8 % (0xFE - 0xA1 + 1);
        UInt8 randomL = 0xB0 + self.uint8 % (0xF7 - 0xB0 + 1);
        UInt32 number = (randomH << 8) + randomL;
        NSData *data = [NSData dataWithBytes:&number length:2];
        [string appendString:[[NSString alloc] initWithData:data encoding:gbkEncoding]];
    }
    return string;
}

- (NSData *)data
{
    return [self dataWithLength:self.length];
}

- (NSData *)dataWithLength:(NSInteger)length
{
    NSMutableData *data = [NSMutableData data];
    [data increaseLengthBy:length];
    unsigned char *bytes = (unsigned char *) data.mutableBytes;
    for (NSUInteger i = 0; i < length; ++i) {
        bytes[i] = (*self.uniformUChar.get())(*self.engine.get());
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
