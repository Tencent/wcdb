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

#include "DatabaseExtendRust.h"

#include "BaseTokenizerUtil.hpp"

void WCDBRustDatabaseClassMethod(configPinyinDict,
                                 const char** keys,
                                 const char*** values,
                                 const size_t* values_len,
                                 size_t keys_len) {
    auto* cppPinyinDict = new WCDB::StringViewMap<std::vector<WCDB::StringView>>();
    for (int i = 0; i < keys_len; i++) {
        const char* key = keys[i];
        WCDB::StringView cppKey(key);
        if (cppKey.empty()) {
            continue;
        }

        std::vector<WCDB::StringView> cppValues;
        size_t row_len = values_len[i];
        const char** row_values = values[i];
        for (int j = 0; j < row_len; j++) {
            const char* row = row_values[j];
            WCDB::StringView cppValue(row);
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

void WCDBRustDatabaseClassMethod(configTraditionalChineseDict,
                                 const char** keys,
                                 const char** values,
                                 size_t len) {
    auto* cppTraditionalChineseDict = new WCDB::StringViewMap<WCDB::StringView>();
    for (int i = 0; i < len; i++) {
        const char* key = keys[i];
        WCDB::StringView cppKey(key);
        if (cppKey.empty()) {
            continue;
        }

        const char* value = values[i];
        WCDB::StringView cppValue(value);
        if (cppValue.empty()) {
            continue;
        }
        cppTraditionalChineseDict->insert_or_assign(cppKey, cppValue);
    }
    WCDB::BaseTokenizerUtil::configTraditionalChineseDict(cppTraditionalChineseDict);
}