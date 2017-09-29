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

#import <WCDB/WCTDatabase+Core.h>
#import <WCDB/WCTDatabase+Private.h>

@implementation WCTDatabase (Core)

+ (NSString *)DefaultBasicConfigName
{
    return @(WCDB::Database::defaultBasicConfigName.c_str());
}

+ (NSString *)DefaultCipherConfigName
{
    return @(WCDB::Database::defaultCipherConfigName.c_str());
}

+ (NSString *)DefaultTraceConfigName
{
    return @(WCDB::Database::defaultTraceConfigName.c_str());
}

+ (NSString *)DefaultCheckpointConfigName
{
    return @(WCDB::Database::defaultCheckpointConfigName.c_str());
}

+ (NSString *)DefaultSynchronousConfigName
{
    return @(WCDB::Database::defaultSynchronousConfigName.c_str());
}

+ (NSString *)DefaultTokenizeConfigName
{
    return @(WCDB::Database::defaultTokenizeConfigName.c_str());
}

- (void)setConfig:(WCDB::Config)invoke forName:(NSString *)name withOrder:(WCDB::Configs::Order)order
{
    _database->setConfig(name.UTF8String, invoke, order);
}

- (void)setConfig:(WCDB::Config)invoke forName:(NSString *)name
{
    _database->setConfig(name.UTF8String, invoke);
}

- (void)setSynchronousFull:(BOOL)full
{
    _database->setSynchronousFull(full);
}

@end
