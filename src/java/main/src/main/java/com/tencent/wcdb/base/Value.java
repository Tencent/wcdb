// Created by qiuwenchen on 2023/4/26.
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

package com.tencent.wcdb.base;

import com.tencent.wcdb.winq.ColumnType;

import org.jetbrains.annotations.Nullable;

import java.util.Arrays;
import java.util.Objects;

public class Value {

    public Value() {
        this.value = null;
    }

    public Value(boolean value) {
        this.value = (long) (value ? 1 : 0);
    }

    public Value(byte value) {
        this.value = (long) value;
    }

    public Value(int value) {
        this.value = (long) value;
    }

    public Value(long value) {
        this.value = value;
    }

    public Value(float value) {
        this.value = (double) value;
    }

    public Value(double value) {
        this.value = value;
    }

    public Value(@Nullable String value) {
        this.value = value;
    }

    public Value(@Nullable byte[] value) {
        this.value = value;
    }

    public Value(@Nullable Object value) {
        if (value == null) {
            this.value = null;
            return;
        }
        Class<?> cls = value.getClass();
        if (cls == Long.class || cls == Double.class || cls == String.class || cls == byte[].class) {
            this.value = value;
        } else if (cls == Integer.class || cls == Short.class || cls == Byte.class) {
            this.value = ((Number) value).longValue();
        } else if (cls == Float.class) {
            this.value = ((Number) value).doubleValue();
        } else {
            this.value = value.toString();
        }
    }

    public Value(@Nullable Value other) {
        this(other == null ? null : other.value);
    }

    public ColumnType getType() {
        if (value == null) return ColumnType.Null;
        Class<?> cls = value.getClass();
        if (cls == Long.class) return ColumnType.Integer;
        if (cls == String.class) return ColumnType.Text;
        if (cls == byte[].class) return ColumnType.BLOB;
        if (cls == Double.class) return ColumnType.Float;
        throw new AssertionError();
    }

    public boolean getBool() {
        return getLong() != 0;
    }

    public byte getByte() {
        return (byte) getLong();
    }

    public short getShort() {
        return (short) getLong();
    }

    public int getInt() {
        return (int) getLong();
    }

    public long getLong() {
        if (value instanceof Long) return (Long) value;
        if (value instanceof Double) return (long) value;
        if (value instanceof String) {
            try {
                return Long.parseLong((String) value);
            } catch (NumberFormatException e) {
            }
        }
        return 0L;
    }

    public float getFloat() {
        return (float) getDouble();
    }

    public double getDouble() {
        if (value instanceof Double) return (Double) value;
        if (value instanceof Long) return (double) value;
        if (value instanceof String) {
            try {
                return Double.parseDouble((String) value);
            } catch (NumberFormatException e) {
            }
        }
        return 0.0;
    }

    @Nullable
    public String getText() {
        if (value instanceof String) return (String) value;
        if (value instanceof byte[]) return new String((byte[]) value);
        return (value == null) ? null : value.toString();
    }

    @Nullable
    public byte[] getBLOB() {
        if (value == null) return null;
        if (value instanceof byte[]) return (byte[]) value;
        return value.toString().getBytes();
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == this || obj == value) return true;
        if (obj instanceof Value) {
            Value v2 = (Value) obj;
            switch (getType()) {
                case Null:
                    return v2.getLong() == 0L;
                case Integer:
                    return ((Long) value) == v2.getLong();
                case Float:
                    return ((Double) value) == v2.getDouble();
                case Text:
                    return value.equals(v2.getText());
                case BLOB:
                    return Arrays.equals((byte[]) value, v2.getBLOB());
            }
            assert false;
        }
        return Objects.deepEquals(obj, value);
    }

    @Override
    public int hashCode() {
        return (value == null) ? 0 : value.hashCode();
    }

    private final Object value;
}
