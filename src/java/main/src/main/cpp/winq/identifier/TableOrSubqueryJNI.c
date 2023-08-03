// Created by qiuwenchen on 2023/4/7.
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

#include "TableOrSubqueryJNI.h"
#include "TableOrSubqueryBridge.h"

jlong WCDBJNITableOrSubqueryObjectMethod(create, WCDBJNIObjectOrStringParameter(tableOrSubquery))
{
    WCDBJNICreateObjectOrStringCommonValue(tableOrSubquery, true);
    jlong ret = (jlong) WCDBTableOrSubqueryCreate(tableOrSubquery_common).innerValue;
    WCDBJNITryReleaseStringInCommonValue(tableOrSubquery);
    return ret;
}

jlong WCDBJNITableOrSubqueryClassMethod(createWithFunction, jstring funcName)
{
    WCDBJNIGetStringCritical(funcName);
    jlong ret = (jlong) WCDBTableOrSubqueryCreateWithFunction(funcNameString).innerValue;
    WCDBJNIReleaseStringCritical(funcName);
    return ret;
}

jlong WCDBJNITableOrSubqueryObjectMethod(createWithTableOrSubqueries,
                                         WCDBJNIObjectOrStringArrayParameter(subquaries))
{
    jlong ret = 0;
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    subquaries,
    ret = (jlong) WCDBTableOrSubqueryCreateWithTableOrSubqueries2(subquaries_commonArray)
          .innerValue) return ret;
}

void WCDBJNITableOrSubqueryObjectMethod(configSchema,
                                        jlong tableOrSubquery,
                                        WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPTableOrSubquery, tableOrSubquery);
    WCDBJNICreateObjectOrStringCommonValue(schema, true);
    WCDBTableOrSubqueryConfigSchema2(tableOrSubqueryStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNITableOrSubqueryObjectMethod(configAlias, jlong tableOrSubquery, jstring alias)
{
    WCDBJNIBridgeStruct(CPPTableOrSubquery, tableOrSubquery);
    WCDBJNIGetStringCritical(alias);
    WCDBTableOrSubqueryConfigAs(tableOrSubqueryStruct, aliasString);
    WCDBJNIReleaseStringCritical(alias);
}

void WCDBJNITableOrSubqueryObjectMethod(configNotIndexed, jlong tableOrSubquery)
{
    WCDBJNIBridgeStruct(CPPTableOrSubquery, tableOrSubquery);
    WCDBTableOrSubqueryConfigNotIndexed(tableOrSubqueryStruct);
}

void WCDBJNITableOrSubqueryObjectMethod(configIndexName, jlong tableOrSubquery, jstring index)
{
    WCDBJNIBridgeStruct(CPPTableOrSubquery, tableOrSubquery);
    WCDBJNIGetStringCritical(index);
    WCDBTableOrSubqueryConfigIndexName(tableOrSubqueryStruct, indexString);
    WCDBJNIReleaseStringCritical(index);
}

void WCDBJNITableOrSubqueryObjectMethod(argument,
                                        jlong tableOrSubquery,
                                        WCDBJNICommonValueParameter(argument))
{
    WCDBJNIBridgeStruct(CPPTableOrSubquery, tableOrSubquery);
    WCDBJNICreateCommonValue(argument, true);
    WCDBTableOrSubqueryConfigArgument(tableOrSubqueryStruct, argument_common);
    WCDBJNITryReleaseStringInCommonValue(argument);
}