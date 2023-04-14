// Created by qiuwenchen on 2023/3/31.
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

#include "BindParameterJNI.h"
#include "BindParameterBridge.h"

jlong WCDBJNIBindParameterObjectMethod(createQuestionSignType, jint num)
{
    return (jlong) WCDBBindparameterCreateQuestionSignType(num).innerValue;
}

jlong WCDBJNIBindParameterObjectMethod(createAtSignType, jstring name)
{
    WCDBJNIGetString(name);
    jlong ret = (jlong) WCDBBindparameterCreateAtSignType(nameString).innerValue;
    WCDBJNIReleaseString(name);
    return ret;
}

jlong WCDBJNIBindParameterObjectMethod(createColonSignType, jstring name)
{
    WCDBJNIGetString(name);
    jlong ret = (jlong) WCDBBindparameterCreateColonSignType(nameString).innerValue;
    WCDBJNIReleaseString(name);
    return ret;
}

jlong WCDBJNIBindParameterObjectMethod(createDollarSignType, jstring name)
{
    WCDBJNIGetString(name);
    jlong ret = (jlong) WCDBBindparameterCreateDollarSignType(nameString).innerValue;
    WCDBJNIReleaseString(name);
    return ret;
}

jlongArray WCDBJNIBindParameterClassMethod(bindParameters, jint size)
{
    jlongArray result = (*env)->NewLongArray(env, size);
    jlong* buffers = alloca(size * sizeof(jlong));
    for (int i = 0; i < size; i++) {
        buffers[i] = (jlong) WCDBBindparameterCreateQuestionSignType(i + 1).innerValue;
    }
    (*env)->SetLongArrayRegion(env, result, 0, size, buffers);
    return result;
}
