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

#import "WCTCompressionInfo+Private.h"
#import "WCTProperty.h"
#import <Foundation/Foundation.h>

@implementation WCTCompressionBaseInfo

- (instancetype)initWithBaseInfo:(const WCDB::CompressionTableBaseInfo &)info
{
    if (self = [super init]) {
        m_baseInfo = &info;
    }
    return self;
}

- (NSString *)table
{
    const WCDB::StringView &table = m_baseInfo->getTable();
    return [[NSString alloc] initWithBytes:table.data() length:table.length() encoding:NSUTF8StringEncoding];
}

@end

@implementation WCTCompressionUserInfo

- (instancetype)initWithUserInfo:(WCDB::CompressionTableUserInfo &)info
{
    if (self = [super initWithBaseInfo:info]) {
        m_userInfo = &info;
    }
    return self;
}

- (void)addZSTDNormalCompressProperty:(const WCTProperty &)property
{
    WCDB::CompressionColumnInfo columnInfo(property, WCDB::CompressionType::Normal);
    m_userInfo->addCompressingColumn(columnInfo);
}

- (void)addZSTDDictCompressProperty:(const WCTProperty &)property
                         withDictId:(WCTDictId)dictId
{
    WCDB::CompressionColumnInfo columnInfo(property, WCDB::CompressionType::Dict);
    columnInfo.setCommonDict(dictId);
    m_userInfo->addCompressingColumn(columnInfo);
}

- (void)addZSTDDictCompressProperty:(const WCTProperty &)property withMatchProperty:(const WCTProperty &)matchProperty andMatchDicts:(nonnull NSDictionary<NSNumber *, NSNumber *> *)dictIds
{
    if (dictIds.count == 0) {
        return;
    }
    __block WCDB::CompressionColumnInfo columnInfo(property, matchProperty);
    [dictIds enumerateKeysAndObjectsUsingBlock:^(NSNumber *value, NSNumber *id_, BOOL *) {
        uint64_t matchValue = value.unsignedLongLongValue;
        WCTDictId dictId = id_.unsignedCharValue;
        if (matchValue == WCTDictDefaultMatchValue) {
            columnInfo.setCommonDict(dictId);
        } else {
            columnInfo.addMatchDict(matchValue, dictId);
        }
    }];
    m_userInfo->addCompressingColumn(columnInfo);
}

- (void)enableReplaceCompression
{
    m_userInfo->enableReplaceCompresssion();
}

@end
