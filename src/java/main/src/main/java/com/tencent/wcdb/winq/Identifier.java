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

import com.tencent.wcdb.base.CppObject;

import org.jetbrains.annotations.NotNull;

class CPPType {
    final static int Invalid = 0;
    final static int Null = 1;
    final static int Bool = 2;
    final static int Int = 3;
    final static int UInt = 4;
    final static int Double = 5;
    final static int String = 6;


    final static int Column = 7;
    final static int Schema = 8;
    final static int ColumnDef = 9;
    final static int ColumnConstraint = 10;
    final static int Expression = 11;
    final static int LiteralValue = 12;
    final static int ForeignKeyClause = 13;
    final static int BindParameter = 14;
    final static int RaiseFunction = 15;
    final static int WindowDef = 16;
    final static int Filter = 17;
    final static int IndexedColumn = 18;
    final static int TableConstraint = 19;
    final static int CommonTableExpression = 20;
    final static int QualifiedTableName = 21;
    final static int OrderingTerm = 22;
    final static int UpsertClause = 23;
    final static int Pragma = 24;
    final static int JoinClause = 25;
    final static int TableOrSubquery = 26;
    final static int JoinConstraint = 27;
    final static int SelectCore = 28;
    final static int ResultColumn = 29;
    final static int FrameSpec = 30;


    final static int AlterTableSTMT = 31;
    final static int AnalyzeSTMT = 32;
    final static int AttachSTMT = 33;
    final static int BeginSTMT = 34;
    final static int CommitSTMT = 35;
    final static int RollbackSTMT = 36;
    final static int SavepointSTMT = 37;
    final static int ReleaseSTMT = 38;
    final static int CreateIndexSTMT = 39;
    final static int CreateTableSTMT = 40;
    final static int CreateTriggerSTMT = 41;
    final static int SelectSTMT = 42;
    final static int InsertSTMT = 43;
    final static int DeleteSTMT = 44;
    final static int UpdateSTMT = 45;
    final static int CreateViewSTMT = 46;
    final static int CreateVirtualTableSTMT = 47;
    final static int DetachSTMT = 48;
    final static int DropIndexSTMT = 49;
    final static int DropTableSTMT = 50;
    final static int DropTriggerSTMT = 51;
    final static int DropViewSTMT = 52;
    final static int PragmaSTMT = 53;
    final static int ReindexSTMT = 54;
    final static int VacuumSTMT = 55;
    final static int ExplainSTMT = 56;
}

public class Identifier extends CppObject implements IdentifierConvertible {
    protected int getType() {
        return CPPType.Invalid;
    }

    protected static int getCppType(Identifier identifier) {
        if (identifier == null) {
            return CPPType.Null;
        }
        return identifier.getType();
    }

    protected static int getCppType(IdentifierConvertible identifier) {
        if (identifier == null) {
            return CPPType.Null;
        }
        return identifier.asIdentifier().getType();
    }

    @NotNull
    public Identifier asIdentifier() {
        return this;
    }

    @NotNull
    public String getDescription() {
        return getDescription(cppObj);
    }

    @Override
    @NotNull
    public String toString() {
        return getDescription();
    }

    static native boolean isWriteStatement(long obj);

    private static native String getDescription(long obj);
}
