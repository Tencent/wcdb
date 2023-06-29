//
// Created by qiuwenchen on 2023/6/29.
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

#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface VideoInfo : NSObject <WCTTableCoding>

@property (nonatomic, strong) NSString* filename;
@property (nonatomic, strong) NSString* clientid;
@property (nonatomic, assign) SInt32 msgsvrid;
@property (nonatomic, assign) SInt32 netoffset;
@property (nonatomic, assign) SInt32 filenowsize;
@property (nonatomic, assign) SInt32 totallen;
@property (nonatomic, assign) SInt32 thumbnetoffset;
@property (nonatomic, assign) SInt32 thumblen;
@property (nonatomic, assign) SInt32 status;
@property (nonatomic, assign) SInt64 createtime;
@property (nonatomic, assign) SInt64 lastmodifytime;
@property (nonatomic, assign) SInt64 downloadtime;
@property (nonatomic, assign) SInt32 videolength;
@property (nonatomic, assign) SInt32 msglocalid;
@property (nonatomic, assign) SInt32 nettimes;
@property (nonatomic, assign) SInt32 cameratype;
@property (nonatomic, strong) NSString* user;
@property (nonatomic, strong) NSString* human;
@property (nonatomic, assign) SInt32 reserved1;
@property (nonatomic, assign) SInt32 reserved2;
@property (nonatomic, strong) NSString* reserved3;
@property (nonatomic, strong) NSString* reserved4;
@property (nonatomic, assign) SInt32 videofuncflag;
@property (nonatomic, assign) SInt64 masssendid;
@property (nonatomic, strong) NSString* masssendlist;
@property (nonatomic, strong) NSString* videomd5;
@property (nonatomic, strong) NSData* streamvideo;
@property (nonatomic, strong) NSString* statextstr;
@property (nonatomic, assign) SInt32 downloadscene;
@property (nonatomic, strong) NSData* mmsightextinfo;
@property (nonatomic, assign) SInt32 preloadsize;
@property (nonatomic, assign) SInt32 videoformat;
@property (nonatomic, assign) SInt32 forward_msg_local_id;
@property (nonatomic, strong) NSString* msg_uuid;
@property (nonatomic, strong) NSString* share_app_info;
@property (nonatomic, strong) NSString* origin_file_name;
@property (nonatomic, assign) SInt32 had_clicked_video;
@property (nonatomic, strong) NSString* media_id;
@property (nonatomic, strong) NSString* media_flag;

WCDB_PROPERTY(filename);
WCDB_PROPERTY(clientid);
WCDB_PROPERTY(msgsvrid);
WCDB_PROPERTY(netoffset);
WCDB_PROPERTY(filenowsize);
WCDB_PROPERTY(totallen);
WCDB_PROPERTY(thumbnetoffset);
WCDB_PROPERTY(thumblen);
WCDB_PROPERTY(status);
WCDB_PROPERTY(createtime);
WCDB_PROPERTY(lastmodifytime);
WCDB_PROPERTY(downloadtime);
WCDB_PROPERTY(videolength);
WCDB_PROPERTY(msglocalid);
WCDB_PROPERTY(nettimes);
WCDB_PROPERTY(cameratype);
WCDB_PROPERTY(user);
WCDB_PROPERTY(human);
WCDB_PROPERTY(reserved1);
WCDB_PROPERTY(reserved2);
WCDB_PROPERTY(reserved3);
WCDB_PROPERTY(reserved4);
WCDB_PROPERTY(videofuncflag);
WCDB_PROPERTY(masssendid);
WCDB_PROPERTY(masssendlist);
WCDB_PROPERTY(videomd5);
WCDB_PROPERTY(streamvideo);
WCDB_PROPERTY(statextstr);
WCDB_PROPERTY(downloadscene);
WCDB_PROPERTY(mmsightextinfo);
WCDB_PROPERTY(preloadsize);
WCDB_PROPERTY(videoformat);
WCDB_PROPERTY(forward_msg_local_id);
WCDB_PROPERTY(msg_uuid);
WCDB_PROPERTY(share_app_info);
WCDB_PROPERTY(origin_file_name);
WCDB_PROPERTY(had_clicked_video);
WCDB_PROPERTY(media_id);
WCDB_PROPERTY(media_flag);

@end
