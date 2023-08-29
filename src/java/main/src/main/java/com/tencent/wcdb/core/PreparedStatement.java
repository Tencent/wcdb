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

import com.tencent.wcdb.base.CppObject;
import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.winq.ColumnType;
import com.tencent.wcdb.winq.Statement;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class PreparedStatement extends CppObject {
    boolean autoFinalize = false;
    PreparedStatement(long cppObj) {
        this.cppObj = cppObj;
    }

    private WCDBException createException() {
        return WCDBException.createException(getError(cppObj));
    }

    private static native long getError(long self);

    void prepare(Statement statement) throws WCDBException {
        if(!prepare(cppObj, CppObject.get(statement))) {
            throw createException();
        }
    }

    private static native boolean prepare(long self, long statement);

    void prepare(String sql) throws WCDBException {
        if(!prepareSQL(cppObj, sql)) {
            throw createException();
        }
    }

    private static native boolean prepareSQL(long self, String sql);

    public boolean checkPrepared() {
        return checkPrepared(cppObj);
    }

    private static native boolean checkPrepared(long self);

    public void step() throws WCDBException {
        if(!step(cppObj)) {
            if(autoFinalize) {
                finalizeStatement();
            }
            throw createException();
        }
    }

    private static native boolean step(long self);

    public void reset() {
        reset(cppObj);
    }

    private static native void reset(long self);

    public void finalizeStatement() {
        finalize(cppObj);
    }

    private static native void finalize(long self);

    public boolean isDone() {
        return isDone(cppObj);
    }

    private static native boolean isDone(long self);

    public void bindBool(boolean value, int index) {
        bindInteger(cppObj, value ? 1 : 0, index);
    }

    public void bindBool(Boolean value, int index) {
        if (value != null) {
            bindInteger(cppObj, value ? 1 : 0, index);
        } else {
            bindNull(index);
        }
    }

    public void bindInteger(char value, int index) {
        bindInteger(cppObj, value, index);
    }

    public void bindInteger(Character value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    public void bindInteger(byte value, int index) {
        bindInteger(cppObj, value, index);
    }

    public void bindInteger(Byte value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    public void bindInteger(short value, int index) {
        bindInteger(cppObj, value, index);
    }

    public void bindInteger(Short value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    public void bindInteger(int value, int index) {
        bindInteger(cppObj, value, index);
    }

    public void bindInteger(Integer value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    public void bindInteger(long value, int index) {
        bindInteger(cppObj, value, index);
    }

    public void bindInteger(Long value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    private static native void bindInteger(long self, long value, int index);

    public void bindDouble(float value, int index) {
        bindDouble(cppObj, value, index);
    }

    public void bindDouble(Float value, int index) {
        if (value != null) {
            bindDouble(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    public void bindDouble(double value, int index) {
        bindDouble(cppObj, value, index);
    }

    public void bindDouble(Double value, int index) {
        if (value != null) {
            bindDouble(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    private static native void bindDouble(long self, double value, int index);

    public void bindText(String value, int index) {
        if(value == null) {
            bindNull(index);
            return;
        }
        bindText(cppObj, value, index);
    }

    private static native void bindText(long self, String value, int index);

    public void bindBLOB(byte[] value, int index) {
        if(value == null) {
            bindNull(index);
            return;
        }
        bindBLOB(cppObj, value, index);
    }

    private static native void bindBLOB(long self, byte[] value, int index);

    public void bindNull(int index) {
        bindNull(cppObj, index);
    }

    private static native void bindNull(long self, int index);

    public void bindValue(Value value, int index) {
        if(value == null) {
            bindNull(index);
            return;
        }
        switch (value.getType()) {
            case Integer:
                bindInteger(value.getLong(), index);
                return;
            case Float:
                bindDouble(value.getDouble(), index);
                return;
            case Text:
                bindText(value.getText(), index);
                return;
            case BLOB:
                bindBLOB(value.getBLOB(), index);
                return;
            case Null:
                bindNull(index);
        }
    }

    public void bindRow(Value[] row) {
        int index = 1;
        for(Value value : row) {
            bindValue(value, index);
            index++;
        }
    }

    public <T> void bindObject(T obj, TableBinding<T> binding) {
        assert obj != null && binding != null;
        if(obj == null || binding == null){
            return;
        }
        int index = 1;
        for(Field<T> field : binding.allBindingFields()) {
            binding.bindField(obj, field, index, this);
            index++;
        }
    }

    public <T> void bindObject(T obj, Field<T> field, int index) {
        if(obj == null || field == null){
            bindNull(index);
            return;
        }
        TableBinding<T> binding = field.getTableBinding();
        assert binding != null;
        binding.bindField(obj, field, index, this);
    }

    public <T> void bindObject(T obj, Field<T>[] fields) {
        assert obj != null && fields != null && fields.length > 0;
        if(obj == null || fields == null || fields.length == 0){
            return;
        }
        int index = 1;
        TableBinding<T> binding = fields[0].getTableBinding();
        assert binding != null;
        for(Field<T> field : fields) {
            binding.bindField(obj, field, index, this);
            index++;
        }
    }

    public int bindParameterIndex(String parameterName) {
        return bindParameterIndex(cppObj, parameterName);
    }

    private static native int bindParameterIndex(long self, String parameterName);

    public ColumnType getColumnType(int index) {
        return ColumnType.valueOf(getColumnType(cppObj, index));
    }

    private static native int getColumnType(long self, int index);

    public boolean getBool(int index) {
        return getInteger(cppObj, index) > 0;
    }

    public char getChar(int index) {
        return (char) getInteger(cppObj, index);
    }

    public byte getByte(int index) {
        return (byte) getInteger(cppObj, index);
    }

    public short getShort(int index) {
        return (short) getInteger(cppObj, index);
    }

    public int getInt(int index) {
        return (int) getInteger(cppObj, index);
    }

    public long getLong(int index) {
        return getInteger(cppObj, index);
    }

    private static native long getInteger(long self, int index);

    public float getFloat(int index) {
        return (float) getDouble(cppObj, index);
    }

    public double getDouble(int index) {
        return getDouble(cppObj, index);
    }

    private static native double getDouble(long self, int index);

    public String getText(int index) {
        return getText(cppObj, index);
    }

    private static native String getText(long self, int index);

    public byte[] getBLOB(int index) {
        return getBLOB(cppObj, index);
    }

    private native byte[] getBLOB(long self, int index);

    public Value getValue(int index) {
        switch (getColumnType(cppObj, index)) {
            case 1:
                return new Value(getInteger(cppObj, index));
            case 2:
                return new Value(getDouble(cppObj, index));
            case 3:
                return new Value(getText(cppObj, index));
            case 4:
                return new Value(getBLOB(cppObj, index));
            default:
                return new Value();
        }
    }

    public Value[] getOneRow() {
        int count = getColumnCount();
        if(count == 0) {
            return new Value[]{};
        }
        Value[] row = new Value[count];
        for(int i = 0; i < count; i++) {
            row[i] = getValue(i);
        }
        return row;
    }

    public List<Value> getOneColumn() throws WCDBException{
        List<Value> column = new ArrayList<Value>();
        step();
        while (!isDone(cppObj)) {
            column.add(getValue(0));
            step();
        }
        return column;
    }

    public List<Integer> getOneColumnInt() throws WCDBException{
        List<Integer> column = new ArrayList<Integer>();
        step();
        while (!isDone(cppObj)) {
            column.add((int) getLong(0));
            step();
        }
        return column;
    }

    public List<Long> getOneColumnLong() throws WCDBException{
        List<Long> column = new ArrayList<Long>();
        step();
        while (!isDone(cppObj)) {
            column.add(getLong(0));
            step();
        }
        return column;
    }

    public List<Float> getOneColumnFloat() throws WCDBException{
        List<Float> column = new ArrayList<Float>();
        step();
        while (!isDone(cppObj)) {
            column.add((float)getDouble(0));
            step();
        }
        return column;
    }

    public List<Double> getOneColumnDouble() throws WCDBException{
        List<Double> column = new ArrayList<Double>();
        step();
        while (!isDone(cppObj)) {
            column.add(getDouble(0));
            step();
        }
        return column;
    }

    public List<String> getOneColumnString() throws WCDBException{
        List<String> column = new ArrayList<String>();
        step();
        while (!isDone(cppObj)) {
            column.add(getText(0));
            step();
        }
        return column;
    }

    public List<byte[]> getOneColumnBLOB() throws WCDBException{
        List<byte[]> column = new ArrayList<byte[]>();
        step();
        while (!isDone(cppObj)) {
            column.add(getBLOB(0));
            step();
        }
        return column;
    }

    public List<Value[]> getMultiRows() throws WCDBException{
        List<Value[]> rows = new ArrayList<Value[]>();
        step();
        while (!isDone(cppObj)) {
            rows.add(getOneRow());
            step();
        }
        return rows;
    }

    public <T> T getOneObject(Field<T>[] fields) {
        return getOneObject(fields, Field.getBindClass(fields));
    }

    public <T, R extends T> R getOneObject(Field<T>[] fields, Class<R> cls) {
        assert fields != null && fields.length > 0;
        if(fields == null || fields.length == 0) {
            return null;
        }

        try {
            return fields[0].getTableBinding().extractObject(fields, this, cls);
        } catch (ReflectiveOperationException e) {
            throw new RuntimeException(e);
        }
    }

    public <T> List<T> getAllObjects(Field<T>[] fields) throws WCDBException {
        return getAllObjects(fields, Field.getBindClass(fields));
    }

    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, Class<R> cls) throws WCDBException {
        assert fields != null && fields.length > 0;
        if(fields == null || fields.length == 0) {
            return Collections.emptyList();
        }
        TableBinding<T> tb = fields[0].getTableBinding();

        List<R> objs = new ArrayList<>();
        step();
        try {
            while (!isDone(cppObj)) {
                objs.add(tb.extractObject(fields, this, cls));
                step();
            }
        } catch (ReflectiveOperationException e) {
            throw new RuntimeException(e);
        }
        return objs;
    }

    public int getColumnCount() {
        return getColumnCount(cppObj);
    }

    private native int getColumnCount(long self);

    public String getColumnName(int index) {
        return getColumnName(cppObj, index);
    }

    private native String getColumnName(long self, int index);

    public String getOriginalColumnName(int index) {
        return getOriginalColumnName(cppObj, index);
    }

    private native String getOriginalColumnName(long self, int index);

    public String getColumnTableName(int index) {
        return getColumnTableName(cppObj, index);
    }

    private native String getColumnTableName(long self, int index);

    public boolean isReadOnly() {
        return isReadOnly(cppObj);
    }

    private native boolean isReadOnly(long self);

}
