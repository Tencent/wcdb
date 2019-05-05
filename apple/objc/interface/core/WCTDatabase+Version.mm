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

#import <WCDB/Macro.hpp>
#import <WCDB/Version.h>
#import <WCDB/WCTDatabase+Version.h>

@implementation WCTDatabase (Version)

+ (NSString *)identifier
{
    return @(WCDB_IDENTIFIER "_" WCDB_VERSION "_" WCDB_STRINGIFY(WCDB_BUILD_TIMESTAMP));
}

+ (NSString *)version
{
    return @WCDB_VERSION;
}

+ (NSString *)commitHash
{
    return @WCDB_COMMIT_HASH;
}

+ (NSUInteger)buildTimestamp
{
    return WCDB_BUILD_TIMESTAMP;
}

+ (NSDate *)buildTime
{
    return [NSDate dateWithTimeIntervalSince1970:WCDB_BUILD_TIMESTAMP];
}

@end
