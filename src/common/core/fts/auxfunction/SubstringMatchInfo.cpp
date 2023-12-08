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

#include "SubstringMatchInfo.hpp"
#include "Assertion.hpp"
#include <cstring>

namespace WCDB {

#pragma mark - MultiLevelHighlight

SubstringMatchInfo::SubstringMatchInfo(void *userContext, FTS5AuxiliaryFunctionAPI &apiObj)
: AbstractFTS5AuxiliaryFunctionObject(userContext, apiObj)
, m_columnNum(0)
, m_phaseMatchResult(nullptr)
, m_substringPhaseMatchCount(0)
, m_currentPhaseMatchCount(0)
, m_phaseCount(0)
, m_matchIndex(nullptr)
, m_tokenPos(0)
, m_bytePos(0)
, m_curLevelStartPos(0)
{
    WCTAssert(apiObj.getValueCount() == 2);
    if (apiObj.getValueCount() != 2) {
        return;
    }
    m_columnNum = (int) apiObj.getIntValue(0);
    m_seperators = apiObj.getTextValue(1);
    m_matchIndex = new int[m_seperators.length()];
}

SubstringMatchInfo::~SubstringMatchInfo()
{
    if (m_phaseMatchResult != nullptr) {
        delete[] m_phaseMatchResult;
    }
    if (m_matchIndex != nullptr) {
        delete[] m_matchIndex;
    }
}

void SubstringMatchInfo::resetStatusFromLevel(int level)
{
    memset(m_phaseMatchResult, 0, m_phaseCount * sizeof(bool));
    m_substringPhaseMatchCount = 0;

    if (level < m_seperators.length()) {
        memset(&m_matchIndex[level], 0, (m_seperators.length() - level) * sizeof(int));
    }
    m_output.clear();
    m_curLevelStartPos = m_bytePos;
}

int SubstringMatchInfo::checkSeperator(char sep)
{
    for (int i = 0; i < m_seperators.length(); i++) {
        if (m_seperators.at(i) == sep) {
            return i;
        }
    }
    return -1;
}

int SubstringMatchInfo::tokenCallback(
void *pContext, int tflags, const char *pToken, int nToken, int iStartOff, int iEndOff)
{
    SubstringMatchInfo *funcObjc = static_cast<SubstringMatchInfo *>(pContext);
    return funcObjc->internalTokenCallback(tflags, pToken, nToken, iStartOff, iEndOff);
}

int SubstringMatchInfo::internalTokenCallback(
int tflags, const char *pToken, int nToken, int iStartOff, int iEndOff)
{
    if (tflags != 0) {
        return FTSError::OK();
    }
    WCDB_UNUSED(nToken);
    int iPos = m_tokenPos;
    int rc = FTSError::OK();
    m_tokenPos++;

    int level = checkSeperator(pToken[0]);
    if (level >= 0) {
        if (m_substringPhaseMatchCount >= m_currentPhaseMatchCount) {
            WCTAssert(m_substringPhaseMatchCount == m_currentPhaseMatchCount);
            m_output.emplace_back(std::make_pair(
            UnsafeStringView(&m_input[m_bytePos], iStartOff - m_bytePos), -1));
            m_bytePos = iStartOff;
            return FTSError::Done();
        } else {
            m_matchIndex[level]++;
            m_bytePos = iEndOff;
            resetStatusFromLevel(level + 1);
        }
    } else if (iPos == m_pIter.m_curPhaseStart) {
        if (iStartOff > m_bytePos) {
            m_output.emplace_back(std::make_pair(
            UnsafeStringView(&m_input[m_bytePos], iStartOff - m_bytePos), -1));
        }
        m_bytePos = iStartOff;
    }
    if (iPos == m_pIter.m_curPhaseEnd) {
        m_output.emplace_back(std::make_pair(
        UnsafeStringView(&m_input[m_bytePos], iEndOff - m_bytePos),
        m_bytePos - m_curLevelStartPos));
        m_bytePos = iEndOff;
        for (int phaseIndex : m_pIter.m_phaseIndexes) {
            if (!m_phaseMatchResult[phaseIndex]) {
                m_phaseMatchResult[phaseIndex] = true;
                m_substringPhaseMatchCount++;
            }
        }
        if (m_pIter.m_iInst < m_pIter.m_nInst
            && m_substringPhaseMatchCount < m_currentPhaseMatchCount) {
            rc = m_pIter.next(m_columnNum);
        } else {
            rc = FTSError::Done();
        }
    }
    return rc;
}

void SubstringMatchInfo::process(FTS5AuxiliaryFunctionAPI &apiObj)
{
    m_input = UnsafeStringView();
    m_tokenPos = 0;
    m_bytePos = 0;
    int rc = FTSError::OK();

    rc = apiObj.getTextForThisRow(m_columnNum, m_input);
    if (m_input.length() > 0) {
        m_phaseCount = apiObj.getPhraseCount();
        if (!m_phaseMatchResult) {
            m_phaseMatchResult = new bool[m_phaseCount];
        }
        m_pIter = PhaseInstIter();
        rc = m_pIter.init(&apiObj);
        if (FTSError::isOK(rc)) {
            m_currentPhaseMatchCount = 0;
            memset(m_phaseMatchResult, 0, m_phaseCount * sizeof(bool));
            rc = m_pIter.next(m_columnNum);
            while (FTSError::isOK(rc)) {
                for (int phaseIndex : m_pIter.m_phaseIndexes) {
                    if (!m_phaseMatchResult[phaseIndex]) {
                        m_phaseMatchResult[phaseIndex] = true;
                        m_currentPhaseMatchCount++;
                    }
                }
                if (m_pIter.m_iInst >= m_pIter.m_nInst) {
                    break;
                }
                rc = m_pIter.next(m_columnNum);
            }
            m_pIter.m_iInst = 0;
            rc = m_pIter.next(m_columnNum);
        }
        resetStatusFromLevel(0);
        if (FTSError::isOK(rc)) {
            rc = apiObj.tokenize(m_input, this, tokenCallback);
        }
        if (m_bytePos < m_input.length()) {
            int i = m_bytePos;
            for (; i < m_input.length(); i++) {
                int level = checkSeperator(m_input[i]);
                if (level >= 0) {
                    m_output.emplace_back(std::make_pair(
                    UnsafeStringView(&m_input[m_bytePos], i - m_bytePos), -1));
                    break;
                }
            }
            if (i == m_input.length()) {
                m_output.emplace_back(std::make_pair(
                UnsafeStringView(&m_input[m_bytePos], i - m_bytePos), -1));
            }
        }
        if ((FTSError::isOK(rc) || FTSError::isDone(rc))
            && m_substringPhaseMatchCount >= m_currentPhaseMatchCount) {
            WCTAssert(m_substringPhaseMatchCount == m_currentPhaseMatchCount);
            std::ostringstream stream;
            generateOutput(stream);
            StringView output = stream.str();
            apiObj.setTextResult(output);
        }
    }
    if (!FTSError::isOK(rc) && !FTSError::isDone(rc)) {
        StringView msg = StringView("parse match info fail");
        apiObj.setErrorResult(rc, msg);
    }
}

void SubstringMatchInfo::generateOutput(std::ostringstream &stream)
{
    for (int i = 0; i < m_seperators.length(); i++) {
        if (i != 0) {
            stream << ',';
        }
        stream << m_matchIndex[i];
    }
    stream << m_seperators[0];
    for (auto &output : m_output) {
        if (output.second >= 0) {
            stream << m_seperators[0];
            stream << output.first;
            stream << m_seperators[0];
            stream << output.second;
            stream << m_seperators[0];
        } else {
            stream << output.first;
        }
    }
}

#pragma mark - PhaseInstIter

SubstringMatchInfo::PhaseInstIter::PhaseInstIter()
: m_curPhaseStart(-1), m_curPhaseEnd(-1), m_iInst(0), m_nInst(0)
{
}

SubstringMatchInfo::PhaseInstIter::~PhaseInstIter() = default;

int SubstringMatchInfo::PhaseInstIter::init(FTS5AuxiliaryFunctionAPI *apiObj)
{
    m_apiObj = apiObj;
    return m_apiObj->instCount(&m_nInst);
}

int SubstringMatchInfo::PhaseInstIter::next(int iCol)
{
    int rc = FTSError::OK();
    m_curPhaseStart = -1;
    m_curPhaseEnd = -1;
    m_phaseIndexes.clear();

    while (FTSError::isOK(rc) && m_iInst < m_nInst) {
        int ic;
        int io;
        int ip;
        rc = m_apiObj->inst(m_iInst, &ip, &ic, &io);
        if (FTSError::isOK(rc)) {
            if (ic == iCol) {
                int iEnd = io - 1 + m_apiObj->getPhraseSize(ip);
                if (m_curPhaseEnd < 0) {
                    m_phaseIndexes.emplace_back(ip);
                    m_curPhaseStart = io;
                    m_curPhaseEnd = iEnd;
                } else if (io <= m_curPhaseEnd) {
                    m_phaseIndexes.emplace_back(ip);
                    if (iEnd > m_curPhaseEnd) {
                        m_curPhaseEnd = iEnd;
                    }
                } else {
                    break;
                }
            }
            m_iInst++;
        }
    }
    return rc;
}

} // namespace WCDB
