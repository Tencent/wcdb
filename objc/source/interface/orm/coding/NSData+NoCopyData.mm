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

#import <WCDB/NSData+NoCopyData.h>
#import <WCDB/String.hpp>

@implementation NSData (WCDBNoCopyData)

+ (instancetype)dataWithNoCopyData:(const WCDB::NoCopyData &)noCopyData
{
    return [[self alloc] initWithBytes:noCopyData.data length:noCopyData.size];
}

- (const WCDB::NoCopyData)noCopyData
{
    static const unsigned char *s_empty_data = (const unsigned char *) WCDB::String::empty().data();
    const unsigned char *bytes = (const unsigned char *) self.bytes;
    return WCDB::NoCopyData(bytes ? bytes : s_empty_data, self.length);
}

@end
