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

#import <WCDB/WCDB.h>

@interface FTS5RowidIndexItem : NSObject <WCTTableCoding>
@property (nonatomic, assign) UInt32 userNameId;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) SInt64 indexRowid;
WCDB_PROPERTY(userNameId);
WCDB_PROPERTY(msgLocalId);
WCDB_PROPERTY(indexRowid);
@end

@interface FTS3MsgContentItem : NSObject <WCTTableCoding>
@property (nonatomic, assign) UInt32 userNameId;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, retain) NSString *msg;

WCDB_PROPERTY(userNameId);
WCDB_PROPERTY(msgLocalId);
WCDB_PROPERTY(createTime);
WCDB_PROPERTY(msg);
@end

@interface FTS5MsgContentItem : NSObject <WCTTableCoding>
@property (nonatomic, assign) UInt32 userNameId;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, retain) NSString *msg;

WCDB_PROPERTY(userNameId);
WCDB_PROPERTY(msgLocalId);
WCDB_PROPERTY(createTime);
WCDB_PROPERTY(msg);
@end

