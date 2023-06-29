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

@WCDBTableCoding(multiIndexes = {
        @MultiIndexes(columns = {"status", "isSend", "createTime"}),
        @MultiIndexes(columns = {"type", "createTime"}),
        @MultiIndexes(columns = {"isSend", "createTime"}),
        @MultiIndexes(columns = {"flag", "msgSeq"})
})
public class Message {
    @WCDBField(isPrimary = true)
    public long msgId;
    @WCDBField
    @WCDBIndex
    public long msgSvrId;
    @WCDBField
    public int type;
    @WCDBField
    public int status;
    @WCDBField
    public int isSend;
    @WCDBField
    public int isShowTimer;
    @WCDBField
    @WCDBIndex
    public long createTime;
    @WCDBField
    public String content;
    @WCDBField
    public String imgPath;
    @WCDBField
    public String reserved;
    @WCDBField
    public byte[] lvBuffer;
    @WCDBField
    public String transContent;
    @WCDBField
    public String transBrandWording;
    @WCDBField
    public String bizClientMsgId;
    @WCDBField
    @WCDBDefault(intValue = -1)
    public long bizChatId;
    @WCDBField
    public String bizChatUserId;
    @WCDBField
    @WCDBIndex
    public long msgSeq;
    @WCDBField
    public int flag;
    @WCDBField
    public byte[] solitaireFoldInfo;
    @WCDBField
    public String historyId;
}
