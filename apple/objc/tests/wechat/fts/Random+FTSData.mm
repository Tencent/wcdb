//
// Created by 陈秋文 on 2020/8/30.
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

#import "Random+FTSData.h"
#import <Foundation/Foundation.h>

@implementation Random (FTSData)

- (NSArray<FTS3MsgContentItem*>*)randomFTS3Items:(NSInteger)count
{
    static int g_localId = 0;
    NSMutableArray<FTS3MsgContentItem*>* itemList = [NSMutableArray arrayWithCapacity:count];
    Random* random = [Random shared];
    for (NSInteger i = 0; i < count; ++i) {
        FTS3MsgContentItem* item = [[FTS3MsgContentItem alloc] init];
        item.msgLocalId = ++g_localId;
        item.createTime = (UInt32)[random.date timeIntervalSince1970];
        item.userNameId = random.uint8 % 100;
        if (i % 3 == 0) {
            item.msg = [random.chineseString stringByReplacingCharactersInRange:NSMakeRange(40, 8) withString:@"大规模批量测试内容"];
        } else {
            item.msg = random.chineseString;
        }
        if (g_localId % 1000 == 0) {
            item.msg = [item.msg stringByReplacingCharactersInRange:NSMakeRange(60, 8) withString:@"中等规模随机命中"];
        }
        if (g_localId % 2000 == 0) {
            item.msg = [item.msg stringByReplacingCharactersInRange:NSMakeRange(0, 7) withString:@"单于骑模具单车"];
        }

        [itemList addObject:item];
    }
    return itemList;
}

- (NSArray<FTS5MsgContentItem*>*)randomFTS5Items:(NSInteger)count
{
    static int g_localId = 900000;
    NSMutableArray<FTS5MsgContentItem*>* itemList = [NSMutableArray arrayWithCapacity:count];
    Random* random = [Random shared];
    for (NSInteger i = 0; i < count; ++i) {
        FTS5MsgContentItem* item = [[FTS5MsgContentItem alloc] init];
        item.msgLocalId = ++g_localId;
        item.createTime = (UInt32)[random.date timeIntervalSince1970];
        item.userNameId = random.uint8 % 100;
        if (i % 3 == 0) {
            item.msg = [random.chineseString stringByReplacingCharactersInRange:NSMakeRange(40, 8) withString:@"大规模批量测试内容"];
        } else {
            item.msg = random.chineseString;
        }
        if (g_localId % 1000 == 0) {
            item.msg = [item.msg stringByReplacingCharactersInRange:NSMakeRange(60, 8) withString:@"中等规模随机命中"];
        }
        if (g_localId % 2000 == 0) {
            item.msg = [item.msg stringByReplacingCharactersInRange:NSMakeRange(0, 7) withString:@"单于骑模具单车"];
        }
        [itemList addObject:item];
    }
    return itemList;
}

- (NSArray<FTS5NewFavSearchItem*>*)randomFavFTSItem:(NSInteger)count
{
    static int g_localId = 0;
    NSMutableArray<FTS5NewFavSearchItem*>* itemList = [NSMutableArray arrayWithCapacity:count];
    Random* random = [Random shared];
    for (NSInteger i = 0; i < count; ++i) {
        FTS5NewFavSearchItem* item = [[FTS5NewFavSearchItem alloc] init];
        item.localId = ++g_localId;
        item.updateTime = (UInt32)[random.date timeIntervalSince1970];
        item.searchTitle = random.chineseString;
        item.searchDesc = random.chineseString;
        item.searchSource = random.chineseString;
        if (i % 3 == 0) {
            item.searchTitle = [item.searchTitle stringByReplacingCharactersInRange:NSMakeRange(40, 8) withString:@"大规模批量测试内容"];
            item.searchDesc = [item.searchDesc stringByReplacingCharactersInRange:NSMakeRange(40, 8) withString:@"大规模批量测试内容"];
            item.searchSource = [item.searchSource stringByReplacingCharactersInRange:NSMakeRange(40, 8) withString:@"大规模批量测试内容"];
        }
        if (g_localId % 1000 == 0) {
            item.searchTitle = [item.searchTitle stringByReplacingCharactersInRange:NSMakeRange(60, 8) withString:@"中等规模随机命中"];
            item.searchDesc = [item.searchDesc stringByReplacingCharactersInRange:NSMakeRange(60, 8) withString:@"中等规模随机命中"];
            item.searchSource = [item.searchSource stringByReplacingCharactersInRange:NSMakeRange(60, 8) withString:@"中等规模随机命中"];
        }
        if (g_localId % 2000 == 0) {
            item.searchTitle = [item.searchTitle stringByReplacingCharactersInRange:NSMakeRange(30, 7) withString:@"单于骑模具单车"];
            item.searchDesc = [item.searchDesc stringByReplacingCharactersInRange:NSMakeRange(30, 7) withString:@"单于骑模具单车"];
            item.searchSource = [item.searchSource stringByReplacingCharactersInRange:NSMakeRange(30, 7) withString:@"单于骑模具单车"];
        }
        [itemList addObject:item];
    }
    return itemList;
}

@end
