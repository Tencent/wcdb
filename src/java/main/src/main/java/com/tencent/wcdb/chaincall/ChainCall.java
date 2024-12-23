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

import org.jetbrains.annotations.NotNull;

public class ChainCall<T extends Statement> {
    Handle handle;
    int changes = 0;
    T statement;
    boolean needChanges = true;
    boolean autoInvalidateHandle = false;

    public ChainCall(@NotNull Handle handle, boolean needChanges, boolean autoInvalidateHandle) {
        this.handle = handle;
        this.needChanges = needChanges;
        this.autoInvalidateHandle = autoInvalidateHandle;
    }

    protected void updateChanges() throws WCDBException {
        if (needChanges) {
            changes = handle.getChanges();
        }
    }

    public void invalidateHandle() {
        if (autoInvalidateHandle) {
            handle.invalidate();
        }
    }

    /**
     * Get the number of changed rows in the most recent call.
     *
     * @return the number of changed rows.
     */
    public int getChanges() {
        return changes;
    }

    /**
     * The statement that `ChainCall` will execute.
     * You can customize this statement directly to implement the capabilities not provided by `ChainCall`.
     *
     * @return the current statement.
     */
    @NotNull
    public T getStatement() {
        return statement;
    }
}
