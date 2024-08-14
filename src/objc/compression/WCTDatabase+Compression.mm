//
// Created by qiuwenchen on 2023/12/2.
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

#import "CompressionCenter.hpp"
#import "CompressionConst.hpp"
#import "CoreConst.h"
#import "WCTCompressionInfo+Private.h"
#import "WCTDatabase+Compression.h"
#import "WCTDatabase+Private.h"
#import <Foundation/Foundation.h>

@implementation WCTDatabase (Compression)

+ (NSData*)trainDictWithStrings:(NSArray<NSString*>*)strings andDictId:(WCTDictId)dictId
{
    int index = 0;
    auto dict = WCDB::CompressionCenter::shared().trainDict(dictId, [&]() -> WCDB::Optional<WCDB::UnsafeData> {
        if (index < strings.count) {
            NSString* string = [strings objectAtIndex:index];
            index++;
            return WCDB::UnsafeData((unsigned char*) string.UTF8String, [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        } else {
            return WCDB::NullOpt;
        }
    });
    if (dict.hasValue()) {
        return [[NSData alloc] initWithBytes:dict->buffer() length:dict->size()];
    }
    return nil;
}

+ (NSData*)trainDictWithDatas:(NSArray<NSData*>*)datas andDictId:(WCTDictId)dictId
{
    int index = 0;
    auto dict = WCDB::CompressionCenter::shared().trainDict(dictId, [&]() -> WCDB::Optional<WCDB::UnsafeData> {
        if (index < datas.count) {
            NSData* data = [datas objectAtIndex:index];
            index++;
            return WCDB::UnsafeData((unsigned char*) data.bytes, data.length);
        } else {
            return WCDB::NullOpt;
        }
    });
    if (dict.hasValue()) {
        return [[NSData alloc] initWithBytes:dict->buffer() length:dict->size()];
    }
    return nil;
}

+ (BOOL)registerZSTDDict:(NSData*)dict andDictId:(WCTDictId)dictId
{
    return WCDB::CompressionCenter::shared().registerDict(dictId, dict);
}

- (void)setCompressionWithFilter:(WCTCompressionFilterBlock)filter
{
    WCDB::InnerDatabase::CompressionTableFilter callback = nullptr;
    if (filter != nil) {
        callback = [filter](WCDB::CompressionTableUserInfo& userInfo) {
            WCTCompressionUserInfo* nsUserInfo = [[WCTCompressionUserInfo alloc] initWithUserInfo:userInfo];
            filter(nsUserInfo);
        };
        WCDB::StringView configName = WCDB::StringView::formatted("%s%s", WCDB::ScalarFunctionConfigPrefix.data(), WCDB::DecompressFunctionName.data());
        _database->setConfig(configName,
                             WCDB::CommonCore::shared().scalarFunctionConfig(WCDB::DecompressFunctionName),
                             WCDB::Configs::Priority::Higher);
    }
    _database->addCompression(callback);
}

- (void)disableCompresssNewData:(BOOL)disable
{
    _database->setCanCompressNewData(!disable);
}

- (BOOL)stepCompression
{
    auto done = _database->stepCompression(false);
    return done.succeed();
}

- (void)enableAutoCompression:(BOOL)flag
{
    WCDB::CommonCore::shared().enableAutoCompress(_database, flag);
}

- (void)setNotificationWhenCompressed:(WCTCompressdNotificationBlock)onCompressd
{
    WCDB::InnerDatabase::CompressedCallback callback = nullptr;
    if (onCompressd != nil) {
        callback = [onCompressd](WCDB::InnerDatabase* database,
                                 const WCDB::CompressionTableBaseInfo* info) {
            WCTCompressionBaseInfo* nsInfo = nil;
            if (info != nil) {
                nsInfo = [[WCTCompressionBaseInfo alloc] initWithBaseInfo:*info];
            }
            WCTDatabase* nsDatabase = [[WCTDatabase alloc] initWithUnsafeDatabase:database];
            onCompressd(nsDatabase, nsInfo);
        };
    }
    _database->setNotificationWhenCompressed(callback);
}

- (BOOL)isCompressed
{
    return _database->isCompressed();
}

- (BOOL)rollbackCompression:(WCTProgressUpdateBlock)onProgressUpdated
{
    WCDB::InnerDatabase::ProgressCallback callback = nullptr;
    if (onProgressUpdated != nil) {
        callback = [onProgressUpdated](double percentage, double increment) {
            return onProgressUpdated(percentage, increment);
        };
    }
    return _database->rollbackCompression(callback);
}

@end
