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

#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTDatabase+Database.h>
#import <WCDB/WCTDatabase+Private.h>

@implementation WCTDatabase (Database)

- (instancetype)initWithPath:(NSString *)path
{
    path = [path stringByStandardizingPath];
    std::shared_ptr<WCDB::CoreBase> core(new WCDB::Database(path.UTF8String));
    if (self = [super initWithCore:core]) {
        _database = (WCDB::Database *) _core.get();
#if TARGET_OS_IPHONE
        _database->setConfig("FileProtection",
                             [path](std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) -> bool {
                                 NSString *shm = [path stringByAppendingString:@"-shm"];
                                 NSFileManager *fm = [NSFileManager defaultManager];
                                 if ([fm fileExistsAtPath:shm]) {
                                     NSDictionary *attributes = [fm attributesOfItemAtPath:shm error:nil];
                                     NSString *fileProtection = [attributes objectForKey:NSFileProtectionKey];
                                     if ([fileProtection isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication] || [fileProtection isEqualToString:NSFileProtectionNone]) {
                                         return true;
                                     }
                                     NSMutableDictionary *newAttributes = [[NSMutableDictionary alloc] initWithDictionary:attributes];
                                     [newAttributes setObject:NSFileProtectionCompleteUntilFirstUserAuthentication forKey:NSFileProtectionKey];
                                     [fm setAttributes:newAttributes ofItemAtPath:path error:nil];
                                 }
                                 return true;
                             });
#endif //TARGET_OS_IPHONE
    }
    return self;
}

- (instancetype)initWithExistingTag:(WCTTag)tag
{
    std::shared_ptr<WCDB::CoreBase> core(new WCDB::Database(tag));
    if (core->getType() != WCDB::CoreType::None) {
        if (self = [super initWithCore:core]) {
            _database = (WCDB::Database *) _core.get();
        }
        return self;
    }
    return nil;
}

- (void)setCipherKey:(NSData *)cipherKey
{
    _database->setCipher(cipherKey.bytes, (int) cipherKey.length);
}

- (void)setCipherKey:(NSData *)cipherKey andCipherPageSize:(int)cipherPageSize
{
    _database->setCipher(cipherKey.bytes, (int) cipherKey.length, cipherPageSize);
}

- (BOOL)canOpen
{
    return _database->canOpen();
}

- (BOOL)isOpened
{
    return _database->isOpened();
}

- (void)blockade
{
    _database->blockade();
}

- (void)unblockade
{
    _database->unblockade();
}

- (void)close
{
    _database->close(nullptr);
}

- (void)close:(WCTCloseBlock)onClosed
{
    std::function<void(void)> callback = nullptr;
    if (onClosed) {
        callback = [onClosed]() {
            onClosed();
        };
    }
    _database->close(callback);
}

- (void)purgeFreeHandles
{
    _database->purgeFreeHandles();
}

+ (void)PurgeFreeHandlesInAllDatabases
{
    WCDB::Database::PurgeFreeHandlesInAllDatabases();
}

- (BOOL)isBlockaded
{
    return _database->isBlockaded();
}

- (void)setTag:(WCDB::Tag)tag
{
    _database->setTag(tag);
}

@end
