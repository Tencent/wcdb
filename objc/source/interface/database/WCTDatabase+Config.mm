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
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTDatabase (Config)

- (void)setCipherKey:(NSData *)cipherKey
{
    WCTRemedialAssert(cipherKey, "Cipher key can't be null.", return;);
    _database->setCipher(cipherKey.noCopyData);
}

- (void)setCipherKey:(NSData *)cipherKey
    andCipherPageSize:(int)cipherPageSize
{
    WCTRemedialAssert(cipherKey, "Cipher key can't be null.", return;);
    _database->setCipher(cipherKey.noCopyData, cipherPageSize);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
          forName:(NSString *)name
     withPriority:(int)priority
{
    WCTRemedialAssert(name, "Config name can't be null.", return;);
    WCTRemedialAssert(nsInvocation, "Use [removeConfigForName:] instead.", return;);
    WCDB::CustomConfig::Invocation invocation = [nsInvocation, self](WCDB::Handle *handle) -> bool {
        WCTHandle *unsafeHandle = [[WCTHandle alloc] initWithDatabase:_database andHandle:handle];
        BOOL result = nsInvocation(unsafeHandle);
        [unsafeHandle finalizeDatabase];
        return result;
    };
    _database->setConfig(std::shared_ptr<WCDB::Config>(new WCDB::CustomConfig(invocation)), name.cppString, priority);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
          forName:(NSString *)name
{
    WCTRemedialAssert(name, "Config name can't be null.", return;);
    WCTRemedialAssert(nsInvocation, "Use [removeConfigForName:] instead.", return;);
    WCDB::CustomConfig::Invocation invocation = [nsInvocation, self](WCDB::Handle *handle) -> bool {
        WCTHandle *unsafeHandle = [[WCTHandle alloc] initWithDatabase:_database andHandle:handle];
        BOOL result = nsInvocation(unsafeHandle);
        [unsafeHandle finalizeDatabase];
        return result;
    };
    _database->setConfig(std::shared_ptr<WCDB::Config>(new WCDB::CustomConfig(invocation)), name.cppString);
}

- (void)removeConfigForName:(NSString *)name
{
    WCTRemedialAssert(name, "Config name can't be null.", return;);
    _database->removeConfig(name.cppString);
}

@end
