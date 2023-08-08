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
    public static final HashMap<String, JavaFieldORMInfo> allInfo = new HashMap<String, JavaFieldORMInfo>(){
        {
            put("boolean", new JavaFieldORMInfo("Integer", "bindBool", "preparedStatement.getBool"));
            put("java.lang.Boolean", new JavaFieldORMInfo("Integer", "bindBool", "preparedStatement.getBool"));
            put("byte", new JavaFieldORMInfo("Integer", "bindInteger", "(byte) preparedStatement.getInteger"));
            put("java.lang.Byte", new JavaFieldORMInfo("Integer", "bindInteger", "(byte) preparedStatement.getInteger"));
            put("char", new JavaFieldORMInfo("Integer", "bindInteger", "(char) preparedStatement.getInteger"));
            put("java.lang.Character", new JavaFieldORMInfo("Integer", "bindInteger", "(char) preparedStatement.getInteger"));
            put("short", new JavaFieldORMInfo("Integer", "bindInteger", "(short) preparedStatement.getInteger"));
            put("java.lang.Short", new JavaFieldORMInfo("Integer", "bindInteger", "(short) preparedStatement.getInteger"));
            put("int", new JavaFieldORMInfo("Integer", "bindInteger", "(int) preparedStatement.getInteger"));
            put("java.lang.Integer", new JavaFieldORMInfo("Integer", "bindInteger", "(int) preparedStatement.getInteger"));
            put("long", new JavaFieldORMInfo("Integer", "bindInteger", "preparedStatement.getInteger"));
            put("java.lang.Long", new JavaFieldORMInfo("Integer", "bindInteger", "preparedStatement.getInteger"));
            put("float", new JavaFieldORMInfo("Float", "bindDouble", "(float) preparedStatement.getDouble"));
            put("java.lang.Float", new JavaFieldORMInfo("Float", "bindDouble", "(float) preparedStatement.getDouble"));
            put("double", new JavaFieldORMInfo("Float", "bindDouble", "preparedStatement.getDouble"));
            put("java.lang.Double", new JavaFieldORMInfo("Float", "bindDouble", "preparedStatement.getDouble"));
            put("java.lang.String", new JavaFieldORMInfo("Text", "bindText", "preparedStatement.getText"));
            put("byte[]", new JavaFieldORMInfo("BLOB", "bindBLOB", "preparedStatement.getBLOB"));
        }
    };

    public static final Set<String> allTypes = allInfo.keySet();

    private JavaFieldORMInfo(String type, String getter, String setter) {
        this.columnType = type;
        this.fieldGetter = getter;
        this.fieldSetter = setter;
    }
    String columnType;
    String fieldGetter;
    String fieldSetter;
}
