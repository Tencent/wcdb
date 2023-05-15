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

#include "ColumnJNI.h"
#include "ColumnBridge.h"

jlong WCDBJNIColumnClassMethodWithNoArg(createAll)
{
    return (jlong) WCDBColumnCreateAll().innerValue;
}

jlong WCDBJNIColumnClassMethodWithNoArg(createRowId)
{
    return (jlong) WCDBColumnCreateRowId().innerValue;
}

jlong WCDBJNIColumnObjectMethod(createWithName, jstring name, jlong binding)
{
    WCDBJNIGetString(name);
    jlong ret
    = (jlong) WCDBColumnCreateWithName2(nameString, (const void *) binding).innerValue;
    WCDBJNIReleaseString(name);
    return ret;
}

jlong WCDBJNIColumnObjectMethod(copy, jlong column)
{
    WCDBJNIBridgeStruct(CPPColumn, column);
    return (jlong) WCDBColumnCopy(columnStruct).innerValue;
}

void WCDBJNIColumnObjectMethod(inTable, jlong column, jstring table)
{
    WCDBJNIGetString(table);
    WCDBJNIBridgeStruct(CPPColumn, column);
    WCDBColumnInTable(columnStruct, tableString);
    WCDBJNIReleaseString(table);
}

void WCDBJNIColumnObjectMethod(inSchema, jlong column, jlong schema)
{
    WCDBJNIBridgeStruct(CPPColumn, column);
    WCDBJNIBridgeStruct(CPPSchema, schema);
    WCDBColumnOfSchema(columnStruct, schemaStruct);
}
