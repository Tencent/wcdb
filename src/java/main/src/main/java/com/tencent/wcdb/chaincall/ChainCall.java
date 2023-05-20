// Created by chenqiuwen on 2023/5/1.
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

package com.tencent.wcdb.chaincall;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.winq.Statement;

public class ChainCall<T extends Statement> {
    Handle handle = null;
    int changes = 0;
    T statement;
    public boolean needChanges = true;
    public boolean autoInvalidateHandle = false;

    public ChainCall(Handle handle) {
        assert handle != null;
        this.handle = handle;
        this.autoInvalidateHandle = autoInvalidateHandle;
    }

    protected void updateChanges() throws WCDBException {
        if(needChanges) {
            changes = handle.getChanges();
        }
    }

    public void invalidateHandle() {
        if(autoInvalidateHandle) {
            handle.invalidate();
        }
    }

    public int getChanges() {
        return changes;
    }

    public T getStatement() {
        return statement;
    }
}
