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

#include "FTSBridge.h"
#include "Assertion.hpp"
#include "CommonCore.hpp"
#include "CustomConfig.hpp"
#include "FTSConst.h"
#include "InnerDatabase.hpp"
#include "ObjectBridge.hpp"
#include "SQLite.h"
#include "TokenizerModule.hpp"

WCDBSwiftTokenizerCreate g_swiftTokenizerCreate = nullptr;
void WCDBSwiftSetTokenizerCreateFunction(_Nonnull WCDBSwiftTokenizerCreate func)
{
    g_swiftTokenizerCreate = func;
}

WCDBSwiftTokenizerLoadInput g_swiftTokenizerLoadInput = nullptr;
void WCDBSwiftSetTokenizerLoadInputFunction(_Nonnull WCDBSwiftTokenizerLoadInput func)
{
    g_swiftTokenizerLoadInput = func;
}

WCDBSwiftTokenizerNextToken g_swiftTokenizerNextToken = nullptr;
void WCDBSwiftSetTokenizerNextTokenFunction(_Nonnull WCDBSwiftTokenizerNextToken func)
{
    g_swiftTokenizerNextToken = func;
}

namespace WCDB {
class AbstractSwiftTokenizer : public AbstractFTSTokenizer {
public:
    AbstractSwiftTokenizer(const char* const* azArg, int nArg, void* pCtx)
    : AbstractFTSTokenizer(azArg, nArg, pCtx), m_swiftTokenizer(nullptr)
    {
        WCTAssert(g_swiftTokenizerCreate != nullptr);
        WCTAssert(g_swiftTokenizerLoadInput != nullptr);
        WCTAssert(g_swiftTokenizerNextToken != nullptr);
    }

    ~AbstractSwiftTokenizer()
    {
        if (m_swiftTokenizer != nullptr) {
            WCDBGetReleaseSwiftObjectFunction()(m_swiftTokenizer);
        }
    }

    void loadInput(const char* pText, int nText, int flags) override final
    {
        if (m_swiftTokenizer) {
            g_swiftTokenizerLoadInput(m_swiftTokenizer, pText, nText, flags);
        }
    }

    int nextToken(const char** ppToken, int* nToken, int* iStart, int* iEnd, int* tflags, int* iPosition) override final
    {
        if (!m_swiftTokenizer) {
            return SQLITE_NOMEM;
        }
        return g_swiftTokenizerNextToken(
        m_swiftTokenizer, ppToken, nToken, iStart, iEnd, tflags, iPosition);
    }

protected:
    SwiftObject* m_swiftTokenizer;
};

template<uint8_t typeId>
class SwiftTokenizer : public AbstractSwiftTokenizer {
    static_assert(typeId < WCDBMaxSwiftTokenizerTypeId, "");

public:
    SwiftTokenizer(const char* const* azArg, int nArg, void* pCtx)
    : AbstractSwiftTokenizer(azArg, nArg, pCtx)
    {
        m_swiftTokenizer = (SwiftObject*) g_swiftTokenizerCreate(typeId, nArg, azArg);
    }
};
} // namespace WCDB

const int WCDBMaxSwiftTokenizerTypeId = 5;

const int WCDBTokenizerErrorCodeOK = SQLITE_OK;
const int WCDBTokenizerErrorCodeDone = SQLITE_DONE;
const int WCDBTokenizerErrorCodeNoMemory = SQLITE_NOMEM;

void WCDBCoreRegisterFTSTokenizer(const char* _Nullable name, int typeId, enum WCDBFTSVersion version)
{
    WCTAssert(version > 2 && version < 6);
    WCTAssert(name != nullptr);
    switch (typeId) {
    case 0: {
        if (version == WCDBFTSVersion5) {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS5TokenizerModuleTemplate<WCDB::SwiftTokenizer<0>>::specializeWithContext(nullptr));
        } else {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS3TokenizerModuleTemplate<WCDB::SwiftTokenizer<0>>::specialize());
        }
    } break;
    case 1: {
        if (version == WCDBFTSVersion5) {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS5TokenizerModuleTemplate<WCDB::SwiftTokenizer<1>>::specializeWithContext(nullptr));
        } else {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS3TokenizerModuleTemplate<WCDB::SwiftTokenizer<1>>::specialize());
        }
    } break;
    case 2: {
        if (version == WCDBFTSVersion5) {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS5TokenizerModuleTemplate<WCDB::SwiftTokenizer<2>>::specializeWithContext(nullptr));
        } else {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS3TokenizerModuleTemplate<WCDB::SwiftTokenizer<2>>::specialize());
        }
    } break;
    case 3: {
        if (version == WCDBFTSVersion5) {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS5TokenizerModuleTemplate<WCDB::SwiftTokenizer<3>>::specializeWithContext(nullptr));
        } else {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS3TokenizerModuleTemplate<WCDB::SwiftTokenizer<3>>::specialize());
        }
    } break;
    case 4: {
        if (version == WCDBFTSVersion5) {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS5TokenizerModuleTemplate<WCDB::SwiftTokenizer<4>>::specializeWithContext(nullptr));
        } else {
            WCDB::CommonCore::shared().registerTokenizer(
            name, WCDB::FTS3TokenizerModuleTemplate<WCDB::SwiftTokenizer<4>>::specialize());
        }
    } break;
    default:
        WCTAssert(false);
        break;
    }
}

void WCDBDatabaseEnableAutoMergeFTS5Index(CPPDatabase database, bool enable)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::CommonCore::shared().enableAutoMergeFTSIndex(cppDatabase, enable);
}

const char* WCDBTokenizerSimpleName()
{
    return WCDB::BuiltinTokenizer::Simple;
}

const char* WCDBTokenizerPorterName()
{
    return WCDB::BuiltinTokenizer::Porter;
}

const char* WCDBTokenizerICUName()
{
    return WCDB::BuiltinTokenizer::ICU;
}

const char* WCDBTokenizerUnicode61Name()
{
    return WCDB::BuiltinTokenizer::Unicode61;
}

const char* WCDBTokenizerOneOrBinaryName()
{
    return WCDB::BuiltinTokenizer::OneOrBinary;
}

const char* WCDBTokenizerLegacyOneOrBinaryName()
{
    return WCDB::BuiltinTokenizer::LegacyOneOrBinary;
}

const char* WCDBTokenizerVerbatimName()
{
    return WCDB::BuiltinTokenizer::Verbatim;
}

const char* WCDBTokenizerPinyinName()
{
    return WCDB::BuiltinTokenizer::Pinyin;
}

const char* WCDBTokenizerParameterNeedSymbolName()
{
    return WCDB::BuiltinTokenizer::Parameter::NeedSymbol;
}

const char* WCDBTokenizerParameterSimplifyChineseName()
{
    return WCDB::BuiltinTokenizer::Parameter::SimplifyChinese;
}

const char* WCDBTokenizerParameterSkipStemmingName()
{
    return WCDB::BuiltinTokenizer::Parameter::SkipStemming;
}

void WCDBDatabaseAddTokenizer(CPPDatabase database, const char* _Nullable tokenizer)
{
    if (tokenizer == nullptr) {
        return;
    }
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::StringView configName = WCDB::StringView::formatted(
    "%s%s", WCDB::TokenizeConfigPrefix.data(), tokenizer);
    cppDatabase->setConfig(configName,
                           WCDB::CommonCore::shared().tokenizerConfig(tokenizer),
                           WCDB::Configs::Priority::Higher);
}

const char* _Nonnull WCDBAuxiliaryFunctionSubstringMatchInfoName()
{
    return WCDB::BuiltinAuxiliaryFunction::SubstringMatchInfo;
}

void WCDBDatabaseAddAuxiliaryFunction(CPPDatabase database, const char* _Nullable auxFunction)
{
    if (auxFunction == nullptr) {
        return;
    }
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::StringView configName = WCDB::StringView::formatted(
    "%s%s", WCDB::AuxiliaryFunctionConfigPrefix.data(), auxFunction);
    cppDatabase->setConfig(configName,
                           WCDB::CommonCore::shared().auxiliaryFunctionConfig(auxFunction),
                           WCDB::Configs::Priority::Higher);
}
