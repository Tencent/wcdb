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

#import <WCDB/Assertion.hpp>
#import <WCDB/Console.hpp>
#import <WCDB/NSObject+WCTColumnCoding.h>

@implementation NSObject (WCTColumnCoding)

+ (instancetype)unarchiveWithWCTValue:(NSData *)value
{
    if (value != nil) {
        if (WCDB::Console::debuggable()) {
            WCTRemedialAssert([self.class conformsToProtocol:@protocol(NSCoding)], "Class should conform to NSCoding or WCTColumnCoding.", return nil;);
        }
        return [NSKeyedUnarchiver unarchiveObjectWithData:value];
    }
    return nil;
}

- (NSData *)archivedWCTValue
{
    if (WCDB::Console::debuggable()) {
        WCTRemedialAssert([self.class conformsToProtocol:@protocol(NSCoding)], "Class should conform to NSCoding or WCTColumnCoding.", return nil;);
    }
    return [NSKeyedArchiver archivedDataWithRootObject:self];
}

+ (WCDB::ColumnType)columnType
{
    return WCTColumnTypeData;
}

@end
