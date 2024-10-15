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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class PreparedStatement extends CppObject {
    boolean autoFinalize = false;
    int columnCount = -1;

    PreparedStatement(long cppObj) {
        this.cppObj = cppObj;
    }

    private WCDBException createException() {
        return WCDBException.createException(getError(cppObj));
    }

    private static native long getError(long self);

    void prepare(Statement statement) throws WCDBException {
        if (!prepare(cppObj, CppObject.get(statement))) {
            throw createException();
        }
    }

    private static native boolean prepare(long self, long statement);

    void prepare(String sql) throws WCDBException {
        if (!prepareSQL(cppObj, sql)) {
            throw createException();
        }
    }

    private static native boolean prepareSQL(long self, String sql);

    /**
     * Check if there is a statement previously prepared.
     *
     * @return True if there is a prepared statement.
     */
    public boolean checkPrepared() {
        return checkPrepared(cppObj);
    }

    private static native boolean checkPrepared(long self);

    /**
     * It is a wrapper for {@code sqlite3_step}.
     *
     * @throws WCDBException if any error occurs.
     */
    public void step() throws WCDBException {
        if (!step(cppObj)) {
            if (autoFinalize) {
                finalizeStatement();
            }
            throw createException();
        }
    }

    private static native boolean step(long self);

    /**
     * It is a wrapper for {@code sqlite3_reset}.
     */
    public void reset() {
        reset(cppObj);
    }

    private static native void reset(long self);

    /**
     * It is a wrapper for {@code sqlite3_clear_bindings}.
     */
    public void clearBindings() {
        clearBindings(cppObj);
    }

    private static native void clearBindings(long self);

    /**
     * Finalize the statement previously prepared.
     */
    public void finalizeStatement() {
        finalize(cppObj);
    }

    private static native void finalize(long self);

    /**
     * Check if you can continue stepping.
     *
     * @return true if you can continue stepping.
     */
    public boolean isDone() {
        return isDone(cppObj);
    }

    private static native boolean isDone(long self);

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindBool(boolean value, int index) {
        bindInteger(cppObj, value ? 1 : 0, index);
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindBool(@Nullable Boolean value, int index) {
        if (value != null) {
            bindInteger(cppObj, value ? 1 : 0, index);
        } else {
            bindNull(index);
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(byte value, int index) {
        bindInteger(cppObj, value, index);
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(@Nullable Byte value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(short value, int index) {
        bindInteger(cppObj, value, index);
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(@Nullable Short value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(int value, int index) {
        bindInteger(cppObj, value, index);
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(@Nullable Integer value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(long value, int index) {
        bindInteger(cppObj, value, index);
    }

    /**
     * The wrapper of {@code sqlite3_bind_int64}.
     */
    public void bindInteger(@Nullable Long value, int index) {
        if (value != null) {
            bindInteger(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    private static native void bindInteger(long self, long value, int index);

    /**
     * The wrapper of {@code sqlite3_bind_double}.
     */
    public void bindDouble(float value, int index) {
        bindDouble(cppObj, value, index);
    }

    /**
     * The wrapper of {@code sqlite3_bind_double}.
     */
    public void bindDouble(@Nullable Float value, int index) {
        if (value != null) {
            bindDouble(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_double}.
     */
    public void bindDouble(double value, int index) {
        bindDouble(cppObj, value, index);
    }

    /**
     * The wrapper of {@code sqlite3_bind_double}.
     */
    public void bindDouble(@Nullable Double value, int index) {
        if (value != null) {
            bindDouble(cppObj, value, index);
        } else {
            bindNull(index);
        }
    }

    private static native void bindDouble(long self, double value, int index);

    /**
     * The wrapper of {@code sqlite3_bind_text}.
     */
    public void bindText(@Nullable String value, int index) {
        if (value == null) {
            bindNull(index);
            return;
        }
        bindText(cppObj, value, index);
    }

    private static native void bindText(long self, String value, int index);

    /**
     * The wrapper of {@code sqlite3_bind_blob}.
     */
    public void bindBLOB(@Nullable byte[] value, int index) {
        if (value == null) {
            bindNull(index);
            return;
        }
        bindBLOB(cppObj, value, index);
    }

    private static native void bindBLOB(long self, byte[] value, int index);

    /**
     * The wrapper of {@code sqlite3_bind_null}.
     */
    public void bindNull(int index) {
        bindNull(cppObj, index);
    }

    private static native void bindNull(long self, int index);

    /**
     * The wrapper of {@code sqlite3_bind_*}.
     * It will call the appropriate routine according to the column type returned by {@code value.getType()}.
     */
    public void bindValue(@Nullable Value value, int index) {
        if (value == null) {
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

    /**
     * The wrapper of {@code sqlite3_bind_*} for binding an array of value.
     * It will call the appropriate routine according to the column type returned by {@code value.getType()}.
     *
     * @param row An array of value.
     */
    public void bindRow(@NotNull Value[] row) {
        int index = 1;
        for (Value value : row) {
            bindValue(value, index);
            index++;
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_*} for binding all fields of object.
     * It will call the appropriate routine according to the data type of field.
     *
     * @param obj     the object to bind.
     * @param binding ORM binding info of the object.
     */
    public <T> void bindObject(@Nullable T obj, @NotNull TableBinding<T> binding) {
        if (obj == null) {
            return;
        }
        int index = 1;
        for (Field<T> field : binding.allBindingFields()) {
            binding.bindField(obj, field, index, this);
            index++;
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_*} for binding field of object to index.
     * It will call the appropriate routine according to the data type of field.
     *
     * @param obj   the object to bind.
     * @param field the specific field to bind.
     */
    public <T> void bindObject(@Nullable T obj, @NotNull Field<T> field, int index) {
        if (obj == null) {
            bindNull(index);
            return;
        }
        TableBinding<T> binding = field.getTableBinding();
        assert binding != null;
        binding.bindField(obj, field, index, this);
    }

    /**
     * The wrapper of {@code sqlite3_bind_*} for binding the specified properties of object.
     * It will call the appropriate routine according to the data type of properties.
     *
     * @param obj    the object to bind.
     * @param fields the specific fields to bind.
     */
    public <T> void bindObject(@Nullable T obj, @NotNull Field<T>[] fields) {
        if (obj == null || fields.length == 0) {
            return;
        }
        int index = 1;
        TableBinding<T> binding = fields[0].getTableBinding();
        assert binding != null;
        for (Field<T> field : fields) {
            binding.bindField(obj, field, index, this);
            index++;
        }
    }

    /**
     * The wrapper of {@code sqlite3_bind_parameter_index}.
     */
    public int bindParameterIndex(String parameterName) {
        return bindParameterIndex(cppObj, parameterName);
    }

    private static native int bindParameterIndex(long self, String parameterName);

    /**
     * The wrapper of {@code sqlite3_column_type}.
     */
    public ColumnType getColumnType(int index) {
        return ColumnType.valueOf(getColumnType(cppObj, index));
    }

    private static native int getColumnType(long self, int index);

    /**
     * The wrapper of {@code sqlite3_column_int64}
     */
    public boolean getBool(int index) {
        return getInteger(cppObj, index) > 0;
    }

    /**
     * The wrapper of {@code sqlite3_column_int64}
     */
    public byte getByte(int index) {
        return (byte) getInteger(cppObj, index);
    }

    /**
     * The wrapper of {@code sqlite3_column_int64}
     */
    public short getShort(int index) {
        return (short) getInteger(cppObj, index);
    }

    /**
     * The wrapper of {@code sqlite3_column_int64}
     */
    public int getInt(int index) {
        return (int) getInteger(cppObj, index);
    }

    /**
     * The wrapper of {@code sqlite3_column_int64}
     */
    public long getLong(int index) {
        return getInteger(cppObj, index);
    }

    private static native long getInteger(long self, int index);

    /**
     * The wrapper of {@code sqlite3_column_double}
     */
    public float getFloat(int index) {
        return (float) getDouble(cppObj, index);
    }

    /**
     * The wrapper of {@code sqlite3_column_double}
     */
    public double getDouble(int index) {
        return getDouble(cppObj, index);
    }

    private static native double getDouble(long self, int index);

    /**
     * The wrapper of {@code sqlite3_column_text}
     */
    @NotNull
    public String getText(int index) {
        return getText(cppObj, index);
    }

    private static native String getText(long self, int index);

    /**
     * The wrapper of {@code sqlite3_column_blob}
     */
    @NotNull
    public byte[] getBLOB(int index) {
        return getBLOB(cppObj, index);
    }

    private static native byte[] getBLOB(long self, int index);

    /**
     * The wrapper of {@code sqlite3_column_*}
     */
    @NotNull
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

    /**
     * Extract all values of current row.
     *
     * @return An array of value.
     */
    @NotNull
    public Value[] getOneRow() {
        int count = getColumnCount();
        if (count == 0) {
            return new Value[]{};
        }
        Value[] row = new Value[count];
        for (int i = 0; i < count; i++) {
            row[i] = getValue(i);
        }
        return row;
    }

    /**
     * Extract all the values of the column of index in the result.
     *
     * @return An list of Value.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<Value> getOneColumn() throws WCDBException {
        List<Value> column = new ArrayList<Value>();
        step();
        while (!isDone(cppObj)) {
            column.add(getValue(0));
            step();
        }
        return column;
    }

    /**
     * Extract all the values of the column of index in the result.
     *
     * @return An list of Integer.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<Integer> getOneColumnInt() throws WCDBException {
        List<Integer> column = new ArrayList<Integer>();
        step();
        while (!isDone(cppObj)) {
            column.add((int) getLong(0));
            step();
        }
        return column;
    }

    /**
     * Extract all the values of the column of index in the result.
     *
     * @return An list of Long.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<Long> getOneColumnLong() throws WCDBException {
        List<Long> column = new ArrayList<Long>();
        step();
        while (!isDone(cppObj)) {
            column.add(getLong(0));
            step();
        }
        return column;
    }

    /**
     * Extract all the values of the column of index in the result.
     *
     * @return An list of Float.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<Float> getOneColumnFloat() throws WCDBException {
        List<Float> column = new ArrayList<Float>();
        step();
        while (!isDone(cppObj)) {
            column.add((float) getDouble(0));
            step();
        }
        return column;
    }

    /**
     * Extract all the values of the column of index in the result.
     *
     * @return An list of Double.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<Double> getOneColumnDouble() throws WCDBException {
        List<Double> column = new ArrayList<Double>();
        step();
        while (!isDone(cppObj)) {
            column.add(getDouble(0));
            step();
        }
        return column;
    }

    /**
     * Extract all the values of the column of index in the result.
     *
     * @return An list of String.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<String> getOneColumnString() throws WCDBException {
        List<String> column = new ArrayList<String>();
        step();
        while (!isDone(cppObj)) {
            column.add(getText(0));
            step();
        }
        return column;
    }

    /**
     * Extract all the values of the column of index in the result.
     *
     * @return An list of Data.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<byte[]> getOneColumnBLOB() throws WCDBException {
        List<byte[]> column = new ArrayList<byte[]>();
        step();
        while (!isDone(cppObj)) {
            column.add(getBLOB(0));
            step();
        }
        return column;
    }

    /**
     * Extract all the values in the result.
     *
     * @return A two-dimensional array of value.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public List<Value[]> getMultiRows() throws WCDBException {
        List<Value[]> rows = new ArrayList<Value[]>();
        step();
        while (!isDone(cppObj)) {
            rows.add(getOneRow());
            step();
        }
        return rows;
    }

    /**
     * Extract the values of the current row and assign them into the specified fields of a new object.
     *
     * @param fields the specified fields.
     * @return an object.
     */
    @NotNull
    public <T> T getOneObject(@NotNull Field<T>[] fields) {
        return getOneObject(fields, Field.getBindClass(fields));
    }

    /**
     * Extract the values of the current row and assign them into the specified fields of a new object.
     *
     * @param fields the specified fields.
     * @param cls    derived class of orm class.
     * @return an object of derived classes of orm class.
     */
    @NotNull
    public <T, R extends T> R getOneObject(@NotNull Field<T>[] fields, @NotNull Class<R> cls) {
        assert fields.length > 0;

        try {
            return fields[0].getTableBinding().extractObject(fields, this, cls);
        } catch (ReflectiveOperationException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Extract the values of all rows in the result and assign them into the specified fields of new objects.
     *
     * @param fields the specified fields.
     * @return an list of objects.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public <T> List<T> getAllObjects(@NotNull Field<T>[] fields) throws WCDBException {
        return getAllObjects(fields, Field.getBindClass(fields));
    }

    /**
     * Extract the values of all rows in the result and assign them into the specified fields of new objects.
     *
     * @param fields the specified fields.
     * @param cls    derived class of orm class.
     * @return a list of objects of derived class of orm class.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull Class<R> cls) throws WCDBException {
        assert fields.length > 0;
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

    /**
     * The wrapper of {@code sqlite3_column_count}.
     *
     * @return number of columns.
     */
    public int getColumnCount() {
        if (columnCount == -1) {
            columnCount = getColumnCount(cppObj);
        }
        return columnCount;
    }

    private static native int getColumnCount(long self);

    /**
     * The wrapper of {@code sqlite3_column_name}.
     *
     * @param index of column.
     * @return name of column.
     */
    @NotNull
    public String getColumnName(int index) {
        return getColumnName(cppObj, index);
    }

    private static native String getColumnName(long self, int index);

    /**
     * The wrapper of {@code sqlite3_column_origin_name}.
     *
     * @param index of column.
     * @return name of column.
     */
    @NotNull
    public String getOriginalColumnName(int index) {
        return getOriginalColumnName(cppObj, index);
    }

    private static native String getOriginalColumnName(long self, int index);

    /**
     * The wrapper of {@code sqlite3_column_table_name}.
     *
     * @param index of column.
     * @return table name.
     */
    @NotNull
    public String getColumnTableName(int index) {
        return getColumnTableName(cppObj, index);
    }

    private static native String getColumnTableName(long self, int index);

    /**
     * The wrapper of {@code sqlite3_stmt_readonly}.
     */
    public boolean isReadOnly() {
        return isReadOnly(cppObj);
    }

    private static native boolean isReadOnly(long self);

}
