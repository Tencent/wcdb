// Created by qiuwenchen on 2023/4/19.
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
package com.tencent.wcdb.core;

import com.tencent.wcdb.winq.ColumnType;

import java.util.ArrayList;

public class PreparedStatement {
//    public boolean checkPrepared() { throw new UnsatisfiedLinkError(); }
//    public void step() throws WCDBException { throw new UnsatisfiedLinkError(); }
//    public void reset() { throw new UnsatisfiedLinkError(); }
//    public void finalizeStatement() { throw new UnsatisfiedLinkError(); }
//    public boolean isDone() { throw new UnsatisfiedLinkError(); }
    public void bindBool(boolean value, int index) { throw new UnsatisfiedLinkError(); }
    public void bindInteger(long value, int index) { throw new UnsatisfiedLinkError(); }
    public void bindDouble(double value, int index) { throw new UnsatisfiedLinkError(); }
    public void bindText(String value, int index) { throw new UnsatisfiedLinkError(); }
    public void bindBLOB(byte[] value, int index) { throw new UnsatisfiedLinkError(); }
    public void bindNull(int index) { throw new UnsatisfiedLinkError(); }
//    public void bindValue(Value value, int index) { throw new UnsatisfiedLinkError(); }
//    public void bindRow(Value[] row) { throw new UnsatisfiedLinkError(); }
//    public <T> void bindObject(T obj, TableBinding<T> binding) { throw new UnsatisfiedLinkError(); }
//    public <T> void bindObject(T obj, Field<T> field, int index) { throw new UnsatisfiedLinkError(); }
//    public <T> void bindObject(T obj, Field<T>[] fields) { throw new UnsatisfiedLinkError(); }
//    public int bindParameterIndex(String parameterName) { throw new UnsatisfiedLinkError(); }
    public ColumnType getColumnType(int index) { throw new UnsatisfiedLinkError(); }
    public boolean getBool(int index) { throw new UnsatisfiedLinkError(); }
    public long getInteger(int index) { throw new UnsatisfiedLinkError(); }
    public double getDouble(int index) { throw new UnsatisfiedLinkError(); }
    public String getText(int index) { throw new UnsatisfiedLinkError(); }
    public byte[] getBLOB(int index) { throw new UnsatisfiedLinkError(); }
//    public Value getValue(int index) { throw new UnsatisfiedLinkError(); }
//    public Value[] getOneRow() { throw new UnsatisfiedLinkError(); }
//    public ArrayList<Value> getOneColumn() throws WCDBException { throw new UnsatisfiedLinkError(); }
//    public ArrayList<Value[]> getMultiRows() throws WCDBException { throw new UnsatisfiedLinkError(); }
//    public <T> T getOneObject(Field<T>[] fields) { throw new UnsatisfiedLinkError(); }
//    public <T> ArrayList<T> getAllObjects(Field<T>[] fields) throws WCDBException { throw new UnsatisfiedLinkError(); }
    public int getColumnCount() { throw new UnsatisfiedLinkError(); }
//    public String getColumnName(int index) { throw new UnsatisfiedLinkError(); }
//    public String getOriginalColumnName(int index) { throw new UnsatisfiedLinkError(); }
//    public String getColumnTableName(int index) { throw new UnsatisfiedLinkError(); }
//    public boolean isReadOnly() { throw new UnsatisfiedLinkError(); }
}
