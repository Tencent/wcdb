//
// Created by 陈秋文 on 2021/10/17.
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

#include <WCDB/AuxiliaryFunctionModule.hpp>
#include <WCDB/StringView.hpp>
#include <vector>

namespace WCDB {

class SubstringMatchInfo : public AbstractFTS5AuxiliaryFunctionObject {
public:
    SubstringMatchInfo(int nVal, sqlite3_value **apVal, void *context);
    ~SubstringMatchInfo();
    void process(const Fts5ExtensionApi *pApi, Fts5Context *pFts, sqlite3_context *pCtx) override final;
    static int
    tokenCallback(void *pContext, int tflags, const char *pToken, int nToken, int iStartOff, int iEndOff);
    int internalTokenCallback(int tflags, const char *pToken, int nToken, int iStartOff, int iEndOff);

private:
    class PhaseInstIter final {
        friend class SubstringMatchInfo;

    public:
        PhaseInstIter();
        ~PhaseInstIter();
        int init(const Fts5ExtensionApi *pApi, Fts5Context *pFts);
        int next(int iCol);

    private:
        const Fts5ExtensionApi *m_pApi;
        Fts5Context *m_pFts;
        int m_curPhaseStart;
        int m_curPhaseEnd;
        int m_iInst;
        int m_nInst;
        std::vector<int> m_phaseIndexes;
    };

    void resetStatusFromLevel(int level);
    int checkSeperator(char sep);
    void generateOutput(std::ostringstream &stream);

    const char *m_input;
    int m_inputLength;
    int m_columnNum;

    bool *m_phaseMatchResult;
    int m_phaseMatchCount;
    int m_phaseCount;

    int *m_matchIndex;

    int m_tokenPos;
    int m_bytePos;
    int m_curLevelStartPos;

    StringView m_seperators;
    std::vector<std::pair<UnsafeStringView, int>> m_output;
    PhaseInstIter m_pIter;
};

} // namespace WCDB
