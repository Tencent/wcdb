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

#include <WCDB/Assertion.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/SQLite.h>
#include <WCDB/SubstringMatchInfo.hpp>

namespace WCDB {

#pragma mark - MultiLevelHighlight

SubstringMatchInfo::SubstringMatchInfo(int nVal, sqlite3_value **apVal, void *context)
: AbstractFTS5AuxiliaryFunctionObject(nVal, apVal, context)
, m_input(nullptr)
, m_inputLength(0)
, m_columnNum(0)
, m_phaseMatchResult(nullptr)
, m_phaseMatchCount(0)
, m_phaseCount(0)
, m_matchIndex(nullptr)
, m_tokenPos(0)
, m_bytePos(0)
, m_curLevelStartPos(0)
{
    WCTAssert(nVal == 2);
    if (nVal != 2) {
        return;
    }
    m_columnNum = sqlite3_value_int(apVal[0]);
    m_seperators = StringView((const char *) sqlite3_value_text(apVal[1]));
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
    m_phaseMatchCount = 0;

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
    if (tflags & FTS5_TOKEN_COLOCATED) {
        return SQLITE_OK;
    }
    WCDB_UNUSED(nToken);
    int iPos = m_tokenPos;
    int rc = SQLITE_OK;
    m_tokenPos++;

    int level = checkSeperator(pToken[0]);
    if (level >= 0) {
        if (m_phaseMatchCount >= m_phaseCount) {
            WCTAssert(m_phaseMatchCount == m_phaseCount);
            m_output.emplace_back(std::make_pair(
            UnsafeStringView(&m_input[m_bytePos], iStartOff - m_bytePos), -1));
            m_bytePos = iStartOff;
            return SQLITE_DONE;
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
                m_phaseMatchCount++;
            }
        }
        if (m_pIter.m_iInst < m_pIter.m_nInst) {
            rc = m_pIter.next(m_columnNum);
        } else {
            rc = SQLITE_DONE;
        }
    }
    return rc;
}

void SubstringMatchInfo::process(const Fts5ExtensionApi *pApi, Fts5Context *pFts, sqlite3_context *pCtx)
{
    m_input = nullptr;
    m_inputLength = 0;
    m_tokenPos = 0;
    m_bytePos = 0;
    int rc = SQLITE_OK;

    rc = pApi->xColumnText(pFts, m_columnNum, &m_input, &m_inputLength);
    if (m_input) {
        m_phaseCount = pApi->xPhraseCount(pFts);
        if (!m_phaseMatchResult) {
            m_phaseMatchResult = new bool[m_phaseCount];
        }
        resetStatusFromLevel(0);
        m_pIter = PhaseInstIter();
        rc = m_pIter.init(pApi, pFts);
        if (rc == SQLITE_OK) {
            rc = m_pIter.next(m_columnNum);
        }
        if (rc == SQLITE_OK) {
            rc = pApi->xTokenize(pFts, m_input, m_inputLength, this, tokenCallback);
        }
        if (m_bytePos < m_inputLength) {
            int i = m_bytePos;
            for (; i < m_inputLength; i++) {
                int level = checkSeperator(m_input[i]);
                if (level >= 0) {
                    m_output.emplace_back(std::make_pair(
                    UnsafeStringView(&m_input[m_bytePos], i - m_bytePos), -1));
                    break;
                }
            }
            if (i == m_inputLength) {
                m_output.emplace_back(std::make_pair(
                UnsafeStringView(&m_input[m_bytePos], i - m_bytePos), -1));
            }
        }
        if ((rc == SQLITE_OK || rc == SQLITE_DONE) && m_phaseMatchCount >= m_phaseCount) {
            WCTAssert(m_phaseMatchCount == m_phaseCount);
            std::ostringstream stream;
            generateOutput(stream);
            sqlite3_result_text(pCtx, stream.str().data(), -1, SQLITE_TRANSIENT);
        }
    }
    if (rc != SQLITE_OK && rc != SQLITE_DONE) {
        sqlite3_result_error_code(pCtx, rc);
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
: m_pApi(nullptr), m_pFts(nullptr), m_curPhaseStart(-1), m_curPhaseEnd(-1), m_iInst(0), m_nInst(0)
{
}

SubstringMatchInfo::PhaseInstIter::~PhaseInstIter() = default;

int SubstringMatchInfo::PhaseInstIter::init(const Fts5ExtensionApi *pApi, Fts5Context *pFts)
{
    m_pApi = pApi;
    m_pFts = pFts;
    return m_pApi->xInstCount(m_pFts, &m_nInst);
}

int SubstringMatchInfo::PhaseInstIter::next(int iCol)
{
    int rc = SQLITE_OK;
    m_curPhaseStart = -1;
    m_curPhaseEnd = -1;
    m_phaseIndexes.clear();

    while (rc == SQLITE_OK && m_iInst < m_nInst) {
        int ic;
        int io;
        int ip;
        rc = m_pApi->xInst(m_pFts, m_iInst, &ip, &ic, &io);
        if (rc == SQLITE_OK) {
            if (ic == iCol) {
                int iEnd = io - 1 + m_pApi->xPhraseSize(m_pFts, ip);
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
