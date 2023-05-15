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

public final class FieldORMInfo {
    public static final HashMap<String, FieldORMInfo> allInfo = new HashMap<String, FieldORMInfo>(){
        {
            put("boolean", new FieldORMInfo("Integer", "bindBool", "preparedStatement.getBool"));
            put("java.lang.Boolean", new FieldORMInfo("Integer", "bindBool", "preparedStatement.getBool"));
            put("byte", new FieldORMInfo("Integer", "bindInteger", "(byte) preparedStatement.getInteger"));
            put("java.lang.Byte", new FieldORMInfo("Integer", "bindInteger", "(byte) preparedStatement.getInteger"));
            put("char", new FieldORMInfo("Integer", "bindInteger", "(char) preparedStatement.getInteger"));
            put("java.lang.Character", new FieldORMInfo("Integer", "bindInteger", "(char) preparedStatement.getInteger"));
            put("short", new FieldORMInfo("Integer", "bindInteger", "(short) preparedStatement.getInteger"));
            put("java.lang.Short", new FieldORMInfo("Integer", "bindInteger", "(short) preparedStatement.getInteger"));
            put("int", new FieldORMInfo("Integer", "bindInteger", "(int) preparedStatement.getInteger"));
            put("java.lang.Integer", new FieldORMInfo("Integer", "bindInteger", "(int) preparedStatement.getInteger"));
            put("long", new FieldORMInfo("Integer", "bindInteger", "preparedStatement.getInteger"));
            put("java.lang.Long", new FieldORMInfo("Integer", "bindInteger", "preparedStatement.getInteger"));
            put("float", new FieldORMInfo("Float", "bindDouble", "(float) preparedStatement.getDouble"));
            put("java.lang.Float", new FieldORMInfo("Float", "bindDouble", "(float) preparedStatement.getDouble"));
            put("double", new FieldORMInfo("Float", "bindDouble", "preparedStatement.getDouble"));
            put("java.lang.Double", new FieldORMInfo("Float", "bindDouble", "preparedStatement.getDouble"));
            put("java.lang.String", new FieldORMInfo("Text", "bindText", "preparedStatement.getText"));
            put("byte[]", new FieldORMInfo("BLOB", "bindBLOB", "preparedStatement.getBLOB"));
        }
    };

    public static final Set<String> allTypes = allInfo.keySet();

    private FieldORMInfo(String type, String getter, String setter) {
        this.columnType = type;
        this.fieldGetter = getter;
        this.fieldSetter = setter;
    }
    String columnType;
    String fieldGetter;
    String fieldSetter;
}
