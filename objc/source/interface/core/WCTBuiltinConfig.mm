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

#import <WCDB/WCTBuiltinConfig.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif //TARGET_OS_IPHONE

#if TARGET_OS_IPHONE
const WCDB::Config WCTBuiltinConfig::fileProtection("FileProtection",
                                                    [](WCDB::Handle *handle) -> bool {
                                                        //TODO use VFS to implement this
                                                        NSString *path = @(handle->path.c_str());
                                                        NSString *shmPath = @(handle->getSHMPath().c_str());
                                                        NSFileManager *fm = [NSFileManager defaultManager];
                                                        if ([fm fileExistsAtPath:shmPath]) {
                                                            NSDictionary *attributes = [fm attributesOfItemAtPath:shmPath error:nil];
                                                            NSString *fileProtection = [attributes objectForKey:NSFileProtectionKey];
                                                            if ([fileProtection isEqualToString:NSFileProtectionCompleteUntilFirstUserAuthentication] || [fileProtection isEqualToString:NSFileProtectionNone]) {
                                                                return true;
                                                            }
                                                            NSMutableDictionary *newAttributes = [[NSMutableDictionary alloc] initWithDictionary:attributes];
                                                            [newAttributes setObject:NSFileProtectionCompleteUntilFirstUserAuthentication forKey:NSFileProtectionKey];
                                                            [fm setAttributes:newAttributes ofItemAtPath:path error:nil];
                                                        }
                                                        return true;
                                                    },
                                                    6);
#endif //TARGET_OS_IPHONE
