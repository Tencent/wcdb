//
// Created by sanhuazhang on 2019/05/02
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

#import "Assertion.hpp"
#import "Core.h"
#import "SQLite.h"
#import "WCTDatabase+Config.h"
#import "WCTDatabase+Private.h"
#import "WCTHandle+Private.h"

static_assert((int) WCTConfigPriorityHighest == (int) WCDB::Configs::Priority::Highest, "");
static_assert((int) WCTConfigPriorityHigh == (int) WCDB::Configs::Priority::High, "");
static_assert((int) WCTConfigPriorityDefault == (int) WCDB::Configs::Priority::Default, "");
static_assert((int) WCTConfigPriorityLow == (int) WCDB::Configs::Priority::Low, "");

@implementation WCTDatabase (Config)

- (void)setCipherKey:(NSData*)cipherKey
{
    [self setCipherKey:cipherKey andCipherPageSize:WCDB::CipherConfigDefaultPageSize];
}

- (void)setCipherKey:(NSData*)cipherKey
   andCipherPageSize:(int)cipherPageSize
{
    [self setCipherKey:cipherKey andCipherPageSize:cipherPageSize andCipherViersion:WCTCipherVersionDefault];
}

- (void)setCipherKey:(NSData* _Nullable)cipherKey
   andCipherPageSize:(int)cipherPageSize
   andCipherViersion:(WCTCipherVersion)cipherVersion
{
    if (cipherKey != nil) {
        _database->setConfig(WCDB::CipherConfigName,
                             std::static_pointer_cast<WCDB::Config>(std::make_shared<WCDB::CipherConfig>(WCDB::UnsafeData::immutable((const unsigned char*) cipherKey.bytes, (size_t) cipherKey.length), cipherPageSize, cipherVersion)),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::CipherConfigName);
    }
}

+ (void)setDefaultCipherConfiguration:(WCTCipherVersion)version
{
    WCDB::Core::shared().setDefaultCipherConfiguration(version);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
 withUninvocation:(WCTConfigBlock)nsUninvocation
          forName:(NSString*)name
     withPriority:(WCTConfigPriority)priority
{
    WCTRemedialAssert(nsInvocation != nil, "Use [removeConfigForName:] instead.", return;);
    WCDB::CustomConfig::Invocation invocation = [nsInvocation, self](WCDB::InnerHandle* handle) -> bool {
        return nsInvocation([[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle]);
    };
    WCDB::CustomConfig::Invocation uninvocation = nullptr;
    if (nsUninvocation != nil) {
        uninvocation = [nsUninvocation, self](WCDB::InnerHandle* handle) -> bool {
            return nsUninvocation([[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle]);
        };
    }
    _database->setConfig(name, std::static_pointer_cast<WCDB::Config>(std::make_shared<WCDB::CustomConfig>(invocation, uninvocation)), priority);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
 withUninvocation:(WCTConfigBlock)nsUninvocation
          forName:(NSString*)name
{
    return [self setConfig:nsInvocation withUninvocation:nsUninvocation forName:name withPriority:WCTConfigPriorityDefault];
}

- (void)removeConfigForName:(NSString*)name
{
    _database->removeConfig(name);
}

+ (void)setABTestConfigWithName:(NSString*)name
                       andValue:(NSString*)value
{
    WCDB::Core::shared().setABTestConfig(name, value);
}

+ (void)removeABTestConfigWithName:(NSString*)name
{
    WCDB::Core::shared().removeABTestConfig(name);
}

+ (NSString*)getABTestConfigWithName:(NSString*)name
{
    WCDB::Optional<WCDB::StringView> value = WCDB::Core::shared().getABTestConfig(name);
    if (value.succeed()) {
        return [NSString stringWithUTF8String:value->data()];
    } else {
        return nil;
    }
}

@end
