// Created by qiuwenchen on 2023/4/23.
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

package com.tencent.wcdb.compiler;

import java.util.HashMap;
import java.util.Set;

public final class JavaFieldORMInfo {
    public static final HashMap<String, JavaFieldORMInfo> allInfo = new HashMap<String, JavaFieldORMInfo>() {
        {
            put("boolean", new JavaFieldORMInfo("Integer", false, "bindBool", "preparedStatement.getBool"));
            put("java.lang.Boolean", new JavaFieldORMInfo("Integer", true, "bindBool", "preparedStatement.getBool"));
            put("byte", new JavaFieldORMInfo("Integer", false, "bindInteger", "preparedStatement.getByte"));
            put("java.lang.Byte", new JavaFieldORMInfo("Integer", true, "bindInteger", "preparedStatement.getByte"));
            put("short", new JavaFieldORMInfo("Integer", false, "bindInteger", "preparedStatement.getShort"));
            put("java.lang.Short", new JavaFieldORMInfo("Integer", true, "bindInteger", "preparedStatement.getShort"));
            put("int", new JavaFieldORMInfo("Integer", false, "bindInteger", "preparedStatement.getInt"));
            put("java.lang.Integer", new JavaFieldORMInfo("Integer", true, "bindInteger", "preparedStatement.getInt"));
            put("long", new JavaFieldORMInfo("Integer", false, "bindInteger", "preparedStatement.getLong"));
            put("java.lang.Long", new JavaFieldORMInfo("Integer", true, "bindInteger", "preparedStatement.getLong"));
            put("float", new JavaFieldORMInfo("Float", false, "bindDouble", "preparedStatement.getFloat"));
            put("java.lang.Float", new JavaFieldORMInfo("Float", true, "bindDouble", "preparedStatement.getFloat"));
            put("double", new JavaFieldORMInfo("Float", false, "bindDouble", "preparedStatement.getDouble"));
            put("java.lang.Double", new JavaFieldORMInfo("Float", true, "bindDouble", "preparedStatement.getDouble"));
            put("java.lang.String", new JavaFieldORMInfo("Text", true, "bindText", "preparedStatement.getText"));
            put("byte[]", new JavaFieldORMInfo("BLOB", true, "bindBLOB", "preparedStatement.getBLOB"));
        }
    };

    public static final Set<String> allTypes = allInfo.keySet();

    private JavaFieldORMInfo(String type, boolean nullable, String getter, String setter) {
        this.columnType = type;
        this.nullable = nullable;
        this.fieldGetter = getter;
        this.fieldSetter = setter;
    }

    String columnType;
    boolean nullable;
    String fieldGetter;
    String fieldSetter;
}
