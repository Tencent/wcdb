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
#import <WCDB/Core.h>
#import <WCDB/WCTDatabase+Config.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTHandle+Private.h>

static_assert((int) WCTConfigPriorityHigh == (int) WCDB::Configs::Priority::High, "");
static_assert((int) WCTConfigPriorityDefault == (int) WCDB::Configs::Priority::Default, "");
static_assert((int) WCTConfigPriorityLow == (int) WCDB::Configs::Priority::Low, "");

NSString* const WCTConfigNameBasic = @WCDB_BASIC_CONFIG_NAME;
NSString* const WCTConfigNameBackup = @WCDB_BACKUP_CONFIG_NAME;
NSString* const WCTConfigNameCheckpoint = @WCDB_CHECKPOINT_CONFIG_NAME;
NSString* const WCTConfigNameTokenize = @WCDB_TOKENIZE_CONFIG_NAME;
NSString* const WCTConfigNameCipher = @WCDB_CIPHER_CONFIG_NAME;
NSString* const WCTConfigNameSQLTrace = @WCDB_SQL_TRACE_CONFIG_NAME;
NSString* const WCTConfigNamePerformanceTrace = @WCDB_PERFORMANCE_TRACE_CONFIG_NAME;
NSString* const WCTConfigNameGlobalSQLTrace = @WCDB_GLOBAL_SQL_TRACE_CONFIG_NAME;
NSString* const WCTConfigNameGlobalPerformanceTrace = @WCDB_GLOBAL_PERFORMANCE_TRACE_CONFIG_NAME;

@implementation WCTDatabase (Config)

- (void)setCipherKey:(NSData*)cipherKey
{
    WCTDatabaseAssert(return;);
    _database->setConfig(WCDB::Core::cipherConfigName,
                         WCDB::Core::shared()->cipherConfig(WCDB::UnsafeData::immutable((const unsigned char*) cipherKey.bytes, (size_t) cipherKey.length)),
                         WCDB::Configs::Priority::Highest);
}

- (void)setCipherKey:(NSData*)cipherKey
   andCipherPageSize:(int)cipherPageSize
{
    WCTDatabaseAssert(return;);
    _database->setConfig(WCDB::Core::cipherConfigName,
                         WCDB::Core::shared()->cipherConfig(WCDB::UnsafeData::immutable((const unsigned char*) cipherKey.bytes, (size_t) cipherKey.length), cipherPageSize),
                         WCDB::Configs::Priority::Highest);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
 withUninvocation:(WCTConfigBlock)nsUninvocation
          forName:(NSString*)name
     withPriority:(int)priority
{
    WCTDatabaseAssert(return;);
    WCTRemedialAssert(nsInvocation, "Use [removeConfigForName:] instead.", return;);
    WCDB::CustomConfig::Invocation invocation = [nsInvocation, self](WCDB::Handle* handle) -> bool {
        return nsInvocation([[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle]);
    };
    WCDB::CustomConfig::Invocation uninvocation = nullptr;
    if (nsUninvocation) {
        uninvocation = [nsUninvocation, self](WCDB::Handle* handle) -> bool {
            return nsUninvocation([[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle]);
        };
    }
    _database->setConfig(name, WCDB::Core::shared()->customConfig(invocation, uninvocation), priority);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
 withUninvocation:(WCTConfigBlock)nsUninvocation
          forName:(NSString*)name
{
    return [self setConfig:nsInvocation withUninvocation:nsUninvocation forName:name withPriority:WCTConfigPriorityDefault];
}

- (void)removeConfigForName:(NSString*)name
{
    WCTDatabaseAssert(return;);
    _database->removeConfig(name);
}

@end
