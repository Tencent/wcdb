//
// Created by qiuwenchen on 2021/10/17.
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

#include "AuxiliaryFunctionModule.hpp"
#include <vector>

namespace WCDB {

class WCDB_API SubstringMatchInfo : public AbstractFTS5AuxiliaryFunctionObject {
public:
    SubstringMatchInfo(void *userContext, FTS5AuxiliaryFunctionAPI &apiObj);
    ~SubstringMatchInfo();
    void process(FTS5AuxiliaryFunctionAPI &apiObj) override final;
    static int
    tokenCallback(void *pContext, int tflags, const char *pToken, int nToken, int iStartOff, int iEndOff);
    int internalTokenCallback(int tflags, const char *pToken, int nToken, int iStartOff, int iEndOff);

private:
    class PhaseInstIter final {
        friend class SubstringMatchInfo;

    public:
        PhaseInstIter();
        ~PhaseInstIter();
        int init(FTS5AuxiliaryFunctionAPI *apiObj);
        int next(int iCol);

    private:
        FTS5AuxiliaryFunctionAPI *m_apiObj;
        int m_curPhaseStart;
        int m_curPhaseEnd;
        int m_iInst;
        int m_nInst;
        std::vector<int> m_phaseIndexes;
    };

    void resetStatusFromLevel(int level);
    int checkSeperator(char sep);
    void generateOutput(std::ostringstream &stream);

    UnsafeStringView m_input;
    int m_columnNum;

    bool *m_phaseMatchResult;
    int m_substringPhaseMatchCount;
    int m_currentPhaseMatchCount;
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
