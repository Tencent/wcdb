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

#import <WCDB/WCDB.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTDatabase (Config)

- (void)setCipherKey:(NSData *)cipherKey
{
    _database->setCipher(cipherKey.bytes, (int) cipherKey.length);
}

- (void)setCipherKey:(NSData *)cipherKey
    andCipherPageSize:(int)cipherPageSize
{
    _database->setCipher(cipherKey.bytes, (int) cipherKey.length, cipherPageSize);
}

- (void)setConfig:(WCTConfigCallback)invoke
          forName:(NSString *)name
        withOrder:(int)order
{
    _database->setConfig(WCDB::Config(name.UTF8String, [invoke, self](WCDB::Handle *handle) -> bool {
        WCTUnsafeHandle *unsafeHandle = [[WCTUnsafeHandle alloc] initWithDatabase:_database andHandle:handle];
        return invoke(unsafeHandle);
    },
                                      order));
}

- (void)setConfig:(WCTConfigCallback)invoke
          forName:(NSString *)name
{
    _database->setConfig(name.UTF8String, [invoke, self](WCDB::Handle *handle) -> bool {
        WCTUnsafeHandle *unsafeHandle = [[WCTUnsafeHandle alloc] initWithDatabase:_database andHandle:handle];
        return invoke(unsafeHandle);
    });
}

@end
