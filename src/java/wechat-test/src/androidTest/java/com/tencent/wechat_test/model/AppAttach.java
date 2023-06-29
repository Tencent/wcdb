// Created by qiuwenchen on 2023/6/28.
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

package com.tencent.wechat_test.model;

import com.tencent.wcdb.*;

@WCDBTableCoding
public class AppAttach {
    @WCDBField
    public String appId;
    @WCDBField
    public long sdkVer;
    @WCDBField
    public String mediaSvrId;
    @WCDBField
    public String mediaId;
    @WCDBField
    public String clientAppDataId;
    @WCDBField
    public long type;
    @WCDBField
    public long totalLen;
    @WCDBField
    public long offset;
    @WCDBField
    @WCDBIndex(name = "appattach_statusIndex")
    public long status;
    @WCDBField
    public long isUpload;
    @WCDBField
    public long createTime;
    @WCDBField
    public long lastModifyTime;
    @WCDBField
    public String fileFullPath;
    @WCDBField
    public String fullXml;
    @WCDBField
    @WCDBIndex(name = "msgInfoIdIndex")
    public long msgInfoId;
    @WCDBField
    public long netTimes;
    @WCDBField
    public long isUseCdn;
    @WCDBField
    public String signature;
    @WCDBField
    public String fakeAeskey;
    @WCDBField
    public String fakeSignature;
}
