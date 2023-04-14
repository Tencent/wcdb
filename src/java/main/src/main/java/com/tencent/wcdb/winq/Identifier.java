// Created by qiuwenchen on 2023/3/30.
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
package com.tencent.wcdb.winq;

import com.tencent.wcdb.util.CPPBridge;

enum CPPType {
    Invalid,
    Null,
    Bool,
    Int,
    UInt,
    Double,
    String,

    Column,
    Schema,
    ColumnDef,
    ColumnConstraint,
    Expression,
    LiteralValue,
    ForeignKeyClause,
    BindParameter,
    RaiseFunction,
    WindowDef,
    Filter,
    IndexedColumn,
    TableConstraint,
    CommonTableExpression,
    QualifiedTableName,
    OrderingTerm,
    UpsertClause,
    Pragma,
    JoinClause,
    TableOrSubquery,
    JoinConstraint,
    SelectCore,
    ResultColumn,
    FrameSpec,

    AlterTableSTMT,
    AnalyzeSTMT,
    AttachSTMT,
    BeginSTMT,
    CommitSTMT,
    RollbackSTMT,
    SavepointSTMT,
    ReleaseSTMT,
    CreateIndexSTMT,
    CreateTableSTMT,
    CreateTriggerSTMT,
    SelectSTMT,
    InsertSTMT,
    DeleteSTMT,
    UpdateSTMT,
    CreateViewSTMT,
    CreateVirtualTableSTMT,
    DetachSTMT,
    DropIndexSTMT,
    DropTableSTMT,
    DropTriggerSTMT,
    DropViewSTMT,
    PragmaSTMT,
    ReindexSTMT,
    VacuumSTMT,
    ExplainSTMT,
}

public class Identifier {
    static {
        System.loadLibrary("wcdb-android");
    }
    protected long cppObj;

    protected CPPType getCppType() {
        return CPPType.Invalid;
    }

    @Override
    protected void finalize() throws Throwable {
        CPPBridge.releaseCPPObject(cppObj);
        super.finalize();
    }

    public Identifier asIdentifier() {
        return this;
    }

    public String getDescription() {
        return getDescription(cppObj);
    }

    private native String getDescription(long obj);
}
