//
// Created by qiuwenchen on 2020/8/30.
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

- (NSArray<FTS5ContactSearchItem*>*)randomContactFTSItem:(NSInteger)count
{
    static int g_localId = 0;
    static NSMutableArray* g_singleContactArr = nil;
    static NSMutableDictionary* g_associateChatRoomDic = nil;
    static NSMutableArray* g_chatroomContactArr = nil;
    static NSMutableDictionary* g_associateMemberDic = nil;

    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        Random* random = [Random shared];
        g_singleContactArr = [[NSMutableArray alloc] init];
        for (int i = 0; i < 1000000; i++) {
            [g_singleContactArr addObject:[@"wxid_" stringByAppendingString:[random stringWithLength:15]]];
        }
        g_chatroomContactArr = [[NSMutableArray alloc] init];
        for (int i = 0; i < 10000; i++) {
            [g_chatroomContactArr addObject:[[random stringWithLength:15] stringByAppendingString:@"@chatroom"]];
        }
        g_associateChatRoomDic = [[NSMutableDictionary alloc] init];
        g_associateMemberDic = [[NSMutableDictionary alloc] init];
        for (NSString* chatroom in g_chatroomContactArr) {
            int memberStart = random.uint32 % 1000000;
            int memberLength = random.uint8;
            NSMutableArray* members = [[NSMutableArray alloc] initWithArray:[g_singleContactArr subarrayWithRange:NSMakeRange(memberStart, MIN(1000000, memberStart + memberLength) - memberStart)]];
            g_associateMemberDic[chatroom] = members;
            for (NSString* member in members) {
                NSMutableArray* associateChatRoom = g_associateChatRoomDic[member];
                if (associateChatRoom == nil) {
                    associateChatRoom = [[NSMutableArray alloc] init];
                    g_associateChatRoomDic[member] = associateChatRoom;
                }
                [associateChatRoom addObject:chatroom];
            }
        }
    });

    NSMutableArray<FTS5ContactSearchItem*>* itemList = [NSMutableArray arrayWithCapacity:count];
    Random* random = [Random shared];

    for (NSInteger i = 0; i < count; ++i) {
        FTS5ContactSearchItem* item = [[FTS5ContactSearchItem alloc] init];
        item.listType = random.uint32;
        item.mainSearchContent = [random chineseStringWithLength:100];
        item.zone = [random stringWithLength:7];
        if (i % 3 == 0) {
            item.mainSearchContent = [item.mainSearchContent stringByReplacingCharactersInRange:NSMakeRange(0, 8) withString:@"大规模批量测试内容"];
        }
        if (g_localId % 1000 == random.uint32 % 1000) {
            for (int j = 0; j < 5; j++) {
                item.mainSearchContent = [item.mainSearchContent stringByReplacingCharactersInRange:NSMakeRange(j * 20, 1) withString:@"\n"];
            }
            for (int j = 0; j < 12; j++) {
                item.mainSearchContent = [item.mainSearchContent stringByReplacingCharactersInRange:NSMakeRange(j * 8, 1) withString:@"\t"];
            }
            item.mainSearchContent = [item.mainSearchContent stringByReplacingCharactersInRange:NSMakeRange(50, 8) withString:@"中等规模随机命中"];
        }
        if (g_localId % 2000 == 0) {
            item.mainSearchContent = [item.mainSearchContent stringByReplacingCharactersInRange:NSMakeRange(0, 7) withString:@"单于骑模具单车"];
        }
        if (g_localId % 100 == 0) {
            item.userName = g_chatroomContactArr[g_localId / 100 % 10000];
            item.groupMembers = [g_associateMemberDic[item.userName] componentsJoinedByString:@";"];
            item.contactType = FTSContactType_Chatroom;
        } else {
            item.userName = g_singleContactArr[g_localId];
            item.associateChatRooms = [g_associateChatRoomDic[item.userName] componentsJoinedByString:@"j"];
            item.contactType = FTSContactType_Normal;
        }
        [itemList addObject:item];
        g_localId++;
    }
    return itemList;
}

@end
