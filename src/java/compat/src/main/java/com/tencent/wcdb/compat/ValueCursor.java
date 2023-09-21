package com.tencent.wcdb.compat;

import android.database.AbstractCursor;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.winq.ColumnType;

import java.util.List;

class ValueCursor extends AbstractCursor {
    private final List<Value[]> mRows;
    private final String[] mColumnNames;

    ValueCursor(List<Value[]> rows, String[] columnNames) {
        mRows = rows;
        mColumnNames = columnNames;
    }

    @Override
    public int getCount() {
        return mRows.size();
    }

    @Override
    public String[] getColumnNames() {
        return mColumnNames;
    }

    @Override
    public int getType(int column) {
        Value[] row = mRows.get(mPos);
        // At this time, type ordinal just match the FIELD_XXX value.
        return row[column].getType().ordinal();
    }

    @Override
    public String getString(int column) {
        Value[] row = mRows.get(mPos);
        return row[column].getText();
    }

    @Override
    public short getShort(int column) {
        Value[] row = mRows.get(mPos);
        return (short) row[column].getLong();
    }

    @Override
    public int getInt(int column) {
        Value[] row = mRows.get(mPos);
        return row[column].getInt();
    }

    @Override
    public long getLong(int column) {
        Value[] row = mRows.get(mPos);
        return row[column].getLong();
    }

    @Override
    public float getFloat(int column) {
        Value[] row = mRows.get(mPos);
        return (float) row[column].getDouble();
    }

    @Override
    public double getDouble(int column) {
        Value[] row = mRows.get(mPos);
        return row[column].getDouble();
    }

    @Override
    public byte[] getBlob(int column) {
        Value[] row = mRows.get(mPos);
        return row[column].getBLOB();
    }

    @Override
    public boolean isNull(int column) {
        Value[] row = mRows.get(mPos);
        return row[column].getType() == ColumnType.Null;
    }
}