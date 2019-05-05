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

#import <WCDB/WCTPerformanceFootprint.h>

@implementation WCTPerformanceFootprint

- (instancetype)initWithSQL:(NSString *)sql
               andFrequency:(unsigned int)frequency
{
    if (self = [super init]) {
        _sql = sql;
        _frequency = frequency;
    }
    return self;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"SQL: %@, Frequency: %u", self.sql, self.frequency];
}

@end
