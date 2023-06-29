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

#import "VideoInfo.h"
#import <Foundation/Foundation.h>

@implementation VideoInfo

WCDB_IMPLEMENTATION(VideoInfo)

WCDB_SYNTHESIZE(filename);
WCDB_SYNTHESIZE(clientid);
WCDB_SYNTHESIZE(msgsvrid);
WCDB_SYNTHESIZE(netoffset);
WCDB_SYNTHESIZE(filenowsize);
WCDB_SYNTHESIZE(totallen);
WCDB_SYNTHESIZE(thumbnetoffset);
WCDB_SYNTHESIZE(thumblen);
WCDB_SYNTHESIZE(status);
WCDB_SYNTHESIZE(createtime);
WCDB_SYNTHESIZE(lastmodifytime);
WCDB_SYNTHESIZE(downloadtime);
WCDB_SYNTHESIZE(videolength);
WCDB_SYNTHESIZE(msglocalid);
WCDB_SYNTHESIZE(nettimes);
WCDB_SYNTHESIZE(cameratype);
WCDB_SYNTHESIZE(user);
WCDB_SYNTHESIZE(human);
WCDB_SYNTHESIZE(reserved1);
WCDB_SYNTHESIZE(reserved2);
WCDB_SYNTHESIZE(reserved3);
WCDB_SYNTHESIZE(reserved4);
WCDB_SYNTHESIZE(videofuncflag);
WCDB_SYNTHESIZE(masssendid);
WCDB_SYNTHESIZE(masssendlist);
WCDB_SYNTHESIZE(videomd5);
WCDB_SYNTHESIZE(streamvideo);
WCDB_SYNTHESIZE(statextstr);
WCDB_SYNTHESIZE(downloadscene);
WCDB_SYNTHESIZE(mmsightextinfo);
WCDB_SYNTHESIZE(preloadsize);
WCDB_SYNTHESIZE(videoformat);
WCDB_SYNTHESIZE(forward_msg_local_id);
WCDB_SYNTHESIZE(msg_uuid);
WCDB_SYNTHESIZE(share_app_info);
WCDB_SYNTHESIZE(origin_file_name);
WCDB_SYNTHESIZE(had_clicked_video);
WCDB_SYNTHESIZE(media_id);
WCDB_SYNTHESIZE(media_flag);

WCDB_PRIMARY(filename)

WCDB_INDEX("_status_downloadtime", status);
WCDB_INDEX("_status_downloadtime", downloadtime);
WCDB_INDEX("_lastmodifytime", lastmodifytime);
WCDB_INDEX("_masssendid", masssendid);
WCDB_INDEX("_media_id", media_id);

@end
