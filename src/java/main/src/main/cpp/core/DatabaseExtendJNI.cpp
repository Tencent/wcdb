// Created by qiuwenchen on 2024/2/21.
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

#include "DatabaseExtendJNI.h"
#include "BaseTokenizerUtil.hpp"

void WCDBJNIDatabaseClassMethod(configPinyinDict, jobjectArray keys, jobjectArray values)
{
    auto* cppPinyinDict = new WCDB::StringViewMap<std::vector<WCDB::StringView>>();
    int count = keys != nullptr ? env->GetArrayLength(keys) : 0;
    for (int i = 0; i < count; i++) {
        auto key = (jstring) env->GetObjectArrayElement(keys, i);
        WCDBJNIGetStringCritical(key);
        WCDB::StringView cppKey(keyString);
        WCDBClearAllPreAllocatedMemory();
        env->ReleaseStringCritical(key, key_utf16String);
        if (cppKey.empty()) {
            continue;
        }

        std::vector<WCDB::StringView> cppValues;
        auto valueArray = (jobjectArray) env->GetObjectArrayElement(values, i);
        int valueCount = valueArray != nullptr ? env->GetArrayLength(valueArray) : 0;
        for (int j = 0; j < valueCount; j++) {
            auto value = (jstring) env->GetObjectArrayElement(valueArray, j);
            WCDBJNIGetStringCritical(value);
            WCDB::StringView cppValue(valueString);
            env->ReleaseStringCritical(value, value_utf16String);
            WCDBClearAllPreAllocatedMemory();
            if (cppValue.empty()) {
                continue;
            }
            cppValues.push_back(cppValue);
        }
        if (cppValues.empty()) {
            continue;
        }
        cppPinyinDict->insert_or_assign(cppKey, cppValues);
    }
    WCDB::BaseTokenizerUtil::configPinyinDict(cppPinyinDict);
}

void WCDBJNIDatabaseClassMethod(configTraditionalChineseDict, jobjectArray keys, jobjectArray values)
{
    auto* cppTraditionalChineseDict = new WCDB::StringViewMap<WCDB::StringView>();
    int count = keys != nullptr ? env->GetArrayLength(keys) : 0;
    for (int i = 0; i < count; i++) {
        auto key = (jstring) env->GetObjectArrayElement(keys, i);
        WCDBJNIGetStringCritical(key);
        WCDB::StringView cppKey(keyString);
        WCDBClearAllPreAllocatedMemory();
        env->ReleaseStringCritical(key, key_utf16String);
        if (cppKey.empty()) {
            continue;
        }

        auto value = (jstring) env->GetObjectArrayElement(values, i);
        WCDBJNIGetStringCritical(value);
        WCDB::StringView cppValue(valueString);
        env->ReleaseStringCritical(value, value_utf16String);
        WCDBClearAllPreAllocatedMemory();
        if (cppValue.empty()) {
            continue;
        }
        cppTraditionalChineseDict->insert_or_assign(cppKey, cppValue);
    }
    WCDB::BaseTokenizerUtil::configTraditionalChineseDict(cppTraditionalChineseDict);
}