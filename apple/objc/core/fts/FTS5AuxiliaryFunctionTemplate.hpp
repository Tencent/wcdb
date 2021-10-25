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
#include <WCDB/SQLite.h>

namespace WCDB {

template<typename AuxiliaryFunctionObject>
class FTS5AuxiliaryFunctionTemplate {
public:
    static_assert(std::is_base_of<AbstractFTS5AuxiliaryFunctionObject, AuxiliaryFunctionObject>::value,
                  "");

    static FTS5AuxiliaryFunctionModule specializeWithContext(void *pCtx)
    {
        return FTS5AuxiliaryFunctionModule(run, pCtx);
    }

    static void run(const Fts5ExtensionApi *pApi,
                    Fts5Context *pFts,
                    sqlite3_context *pCtx,
                    int nVal,
                    sqlite3_value **apVal)
    {
        AbstractFTS5AuxiliaryFunctionObject *funcObject
        = (AbstractFTS5AuxiliaryFunctionObject *) pApi->xGetAuxdata(pFts, false);
        if (funcObject == nullptr) {
            void *userData = pApi->xUserData(pFts);
            funcObject = static_cast<AbstractFTS5AuxiliaryFunctionObject *>(
            new AuxiliaryFunctionObject(nVal, apVal, userData));
            pApi->xSetAuxdata(pFts, funcObject, deleteAuxData);
        }
        funcObject->process(pApi, pFts, pCtx);
    }

    static void deleteAuxData(void *auxObject)
    {
        if (auxObject != nullptr) {
            delete static_cast<AuxiliaryFunctionObject *>(auxObject);
        }
    }
};

} // namespace WCDB
