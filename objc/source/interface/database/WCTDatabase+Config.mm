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
    _database->setCipher(cipherKey.noCopyData);
}

- (void)setCipherKey:(NSData *)cipherKey
    andCipherPageSize:(int)cipherPageSize
{
    _database->setCipher(cipherKey.noCopyData, cipherPageSize);
}

- (void)setConfig:(WCTConfigBlock)invoke
          forName:(NSString *)name
        withOrder:(int)order
{
    WCDB::Config::Callback callback = nullptr;
    if (invoke) {
        callback = [invoke, self](WCDB::Handle *handle) -> bool {
            WCTHandle *unsafeHandle = [[WCTHandle alloc] initWithDatabase:_database andHandle:handle];
            BOOL result = invoke(unsafeHandle);
            [unsafeHandle finalizeDatabase];
            return result;
        };
    }
    _database->setConfig(WCDB::Config(name.cppString, callback, order));
}

- (void)setConfig:(WCTConfigBlock)invoke
          forName:(NSString *)name
{
    WCDB::Config::Callback callback = nullptr;
    if (invoke) {
        callback = [invoke, self](WCDB::Handle *handle) -> bool {
            WCTHandle *unsafeHandle = [[WCTHandle alloc] initWithDatabase:_database andHandle:handle];
            BOOL result = invoke(unsafeHandle);
            [unsafeHandle finalizeDatabase];
            return result;
        };
    }
    _database->setConfig(name.cppString, callback);
}

@end
