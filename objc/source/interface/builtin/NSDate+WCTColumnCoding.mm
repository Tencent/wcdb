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

#import <WCDB/Interface.h>

static BOOL s_compatible_for_1_0_x = NO;

@implementation NSDate (ColumnCoding)

+ (BOOL)compatible_for_1_0_x
{
    return s_compatible_for_1_0_x;
}

+ (void)setCompatible_for_1_0_x:(BOOL)flag
{
    s_compatible_for_1_0_x = flag;
}

+ (instancetype)unarchiveWithWCTValue:(NSNumber *)value
{
    if (!value) {
        return nil;
    }
    if (!s_compatible_for_1_0_x) {
        return [NSDate dateWithTimeIntervalSince1970:value.doubleValue];
    } else {
        return [NSDate dateWithTimeIntervalSince1970:value.longLongValue];
    }
}

- (NSNumber *)archivedWCTValue
{
    if (!s_compatible_for_1_0_x) {
        return [NSNumber numberWithDouble:self.timeIntervalSince1970];
    } else {
        return [NSNumber numberWithLongLong:self.timeIntervalSince1970];
    }
}

+ (WCDB::ColumnType)columnType
{
    if (!s_compatible_for_1_0_x) {
        return WCTColumnTypeFloat;
    } else {
        return WCTColumnTypeInteger64;
    }
}

@end
