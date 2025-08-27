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

#include "BindParameterRust.h"

#include "BindParameterBridge.h"

void* WCDBRustBindParameterClassMethod(createQuestionSignType, int num) {
    return WCDBBindparameterCreateQuestionSignType(num).innerValue;
}

void* WCDBRustBindParameterClassMethod(createAtSignType, const char* name) {
    // WCDBRustGetStringCritical(name);
    void* ret = WCDBBindparameterCreateAtSignType(name).innerValue;
    // WCDBRustReleaseStringCritical(name);
    return ret;
}

void* WCDBRustBindParameterClassMethod(createColonSignType, const char* name) {
    // WCDBRustGetStringCritical(name);
    void* ret = WCDBBindparameterCreateColonSignType(name).innerValue;
    // WCDBRustReleaseStringCritical(name);
    return ret;
}

void* WCDBRustBindParameterClassMethod(createDollarSignType, const char* name) {
    // WCDBRustGetStringCritical(name);
    void* ret = WCDBBindparameterCreateDollarSignType(name).innerValue;
    // WCDBRustReleaseStringCritical(name);
    return ret;
}

void** WCDBRustBindParameterClassMethod(bindParameters, int size) {
    void** ret = malloc(sizeof(void*) * size);
    for (int i = 0; i < size; i++) {
        ret[i] = WCDBBindparameterCreateQuestionSignType(i + 1).innerValue;
    }
    return ret;
}