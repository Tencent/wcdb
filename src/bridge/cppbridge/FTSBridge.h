//
// Created by qiuwenchen on 2022/11/3.
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

#pragma once
#include "DatabaseBridge.h"

WCDB_EXTERN_C_BEGIN

typedef SwiftObject* _Nullable (*WCDBSwiftTokenizerCreate)(
int typeId, int argc, const char* _Nonnull const* _Nullable argv);
void WCDBSwiftSetTokenizerCreateFunction(_Nonnull WCDBSwiftTokenizerCreate func);

typedef void (*WCDBSwiftTokenizerLoadInput)(SwiftObject* _Nonnull obj,
                                            const char* _Nullable input,
                                            int length,
                                            int flags);
void WCDBSwiftSetTokenizerLoadInputFunction(_Nonnull WCDBSwiftTokenizerLoadInput func);

typedef int (*WCDBSwiftTokenizerNextToken)(SwiftObject* _Nonnull obj,
                                           const char* _Nullable* _Nonnull ppToken,
                                           int* _Nonnull nToken,
                                           int* _Nonnull iStart,
                                           int* _Nonnull iEnd,
                                           int* _Nullable tflags,
                                           int* _Nullable iPosition);
void WCDBSwiftSetTokenizerNextTokenFunction(_Nonnull WCDBSwiftTokenizerNextToken func);

extern const int WCDBTokenizerErrorCodeOK;
extern const int WCDBTokenizerErrorCodeDone;
extern const int WCDBTokenizerErrorCodeNoMemory;

extern const int WCDBMaxSwiftTokenizerTypeId;

enum WCDBFTSVersion {
    WCDBFTSVersion3 = 3,
    WCDBFTSVersion4 = 4,
    WCDBFTSVersion5 = 5,
};

void WCDBCoreRegisterFTSTokenizer(const char* _Nullable name, int typeId, enum WCDBFTSVersion ftsVersion);

void WCDBDatabaseEnableAutoMergeFTS5Index(CPPDatabase database, bool enable);

const char* _Nonnull WCDBTokenizerSimpleName();
const char* _Nonnull WCDBTokenizerPorterName();
const char* _Nonnull WCDBTokenizerICUName();
const char* _Nonnull WCDBTokenizerUnicode61Name();
const char* _Nonnull WCDBTokenizerOneOrBinaryName();
const char* _Nonnull WCDBTokenizerLegacyOneOrBinaryName();
const char* _Nonnull WCDBTokenizerVerbatimName();
const char* _Nonnull WCDBTokenizerPinyinName();
const char* _Nonnull WCDBTokenizerParameterNeedSymbolName();
const char* _Nonnull WCDBTokenizerParameterSimplifyChineseName();
const char* _Nonnull WCDBTokenizerParameterSkipStemmingName();
void WCDBDatabaseAddTokenizer(CPPDatabase database, const char* _Nullable tokenizer);

const char* _Nonnull WCDBAuxiliaryFunctionSubstringMatchInfoName();

void WCDBDatabaseAddAuxiliaryFunction(CPPDatabase database, const char* _Nullable auxFunction);

WCDB_EXTERN_C_END
