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

extern SwiftObject* _Nullable (*_Nullable WCDBSwiftTokenizerCreate)(
int typeId, int argc, const char* _Nonnull const* _Nullable argv);
extern void (*_Nullable WCDBSwiftTokenizerLoadInput)(SwiftObject* _Nonnull obj,
                                                     const char* _Nullable input,
                                                     int length,
                                                     int flags);
extern int (*_Nullable WCDBSwiftTokenizerNextToken)(SwiftObject* _Nonnull obj,
                                                    const char* _Nullable* _Nonnull ppToken,
                                                    int* _Nonnull nToken,
                                                    int* _Nonnull iStart,
                                                    int* _Nonnull iEnd,
                                                    int* _Nullable tflags,
                                                    int* _Nullable iPosition);

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

extern const char* _Nonnull WCDBTokenizerSimple;
extern const char* _Nonnull WCDBTokenizerPorter;
extern const char* _Nonnull WCDBTokenizerICU;
extern const char* _Nonnull WCDBTokenizerUnicode61;
extern const char* _Nonnull WCDBTokenizerOneOrBinary;
extern const char* _Nonnull WCDBTokenizerLegacyOneOrBinary;
extern const char* _Nonnull WCDBTokenizerVerbatim;
extern const char* _Nonnull WCDBTokenizerPinyin;
extern const char* _Nonnull WCDBTokenizerParameter_NeedSymbol;
extern const char* _Nonnull WCDBTokenizerParameter_SimplifyChinese;
extern const char* _Nonnull WCDBTokenizerParameter_SkipStemming;
void WCDBDatabaseAddTokenizer(CPPDatabase database, const char* _Nullable tokenizer);

extern const char* _Nonnull WCDBAuxiliaryFunction_SubstringMatchInfo;
void WCDBDatabaseAddAuxiliaryFunction(CPPDatabase database, const char* _Nullable auxFunction);

WCDB_EXTERN_C_END
