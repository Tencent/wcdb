// Created by qiuwenchen on 2023/6/27.
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
public class ImgInfo {
    @WCDBField(isPrimary = true)
    public long id;
    @WCDBField
    @WCDBIndex(name = "serverImgInfoIndex")
    public long msgSvrId;
    @WCDBField
    public int offset;
    @WCDBField
    public int totalLen;
    @WCDBField
    public String bigImgPath;
    @WCDBField
    public String thumbImgPath;
    @WCDBField
    public int createtime;
    @WCDBField
    @WCDBIndex(name = "msgLocalIdIndex")
    public int msglocalid;
    @WCDBField
    public int status;
    @WCDBField
    public int nettimes;
    @WCDBField
    @WCDBIndex(name = "serverImgInfoHdIndex")
    public int reserved1;
    @WCDBField
    public int reserved2;
    @WCDBField
    public String reserved3;
    @WCDBField
    public String reserved4;
    @WCDBField
    @WCDBDefault(intValue = 0)
    public int hashdthumb;
    @WCDBField
    @WCDBDefault(intValue = 1)
    @WCDBIndex
    public int iscomplete;
    @WCDBField
    @WCDBIndex
    public String origImgMD5;
    @WCDBField
    @WCDBDefault(intValue = 0)
    public int compressType;
    @WCDBField
    public String midImgPath;
    @WCDBField
    @WCDBDefault(intValue = 0)
    public int forwardType;
    @WCDBField
    public String hevcPath;
    @WCDBField
    @WCDBDefault(intValue = 0)
    public int sendImgType;
}
