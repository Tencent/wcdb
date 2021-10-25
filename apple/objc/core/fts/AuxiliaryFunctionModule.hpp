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

typedef struct sqlite3_value sqlite3_value;
typedef struct Fts5ExtensionApi Fts5ExtensionApi;
typedef struct Fts5Context Fts5Context;
typedef struct sqlite3_context sqlite3_context;

namespace WCDB {

class AbstractFTS5AuxiliaryFunctionObject {
public:
    AbstractFTS5AuxiliaryFunctionObject(int nVal, sqlite3_value **apVal, void *context);
    virtual ~AbstractFTS5AuxiliaryFunctionObject() = 0;
    virtual void
    process(const Fts5ExtensionApi *pApi, Fts5Context *pFts, sqlite3_context *pCtx)
    = 0;
};

class FTS5AuxiliaryFunctionModule final {
public:
    typedef void (*AuxiliaryFunction)(const Fts5ExtensionApi *pApi,
                                      Fts5Context *pFts,
                                      sqlite3_context *pCtx,
                                      int nVal,
                                      sqlite3_value **apVal);
    FTS5AuxiliaryFunctionModule();
    FTS5AuxiliaryFunctionModule(const AuxiliaryFunction &func, void *context = nullptr);
    AuxiliaryFunction getFunc() const;
    void *getContext() const;

private:
    AuxiliaryFunction m_func;
    void *m_context;
};

} // namespace WCDB
