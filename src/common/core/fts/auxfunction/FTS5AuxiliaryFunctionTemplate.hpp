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

namespace WCDB {

template<typename AuxiliaryFunctionObject>
class FTS5AuxiliaryFunctionTemplate {
public:
    static_assert(std::is_base_of<AbstractFTS5AuxiliaryFunctionObject, AuxiliaryFunctionObject>::value,
                  "");

    static FTS5AuxiliaryFunctionModule specializeWithContext(void *userContext)
    {
        return FTS5AuxiliaryFunctionModule(run, userContext);
    }

    static void run(const FTS5AuxiliaryFunctionAPIPointer *pApi,
                    FTS5AuxiliaryFunctionContext *pFts,
                    SQLiteContext *pCtx,
                    int nVal,
                    SQLiteValue **apVal)
    {
        FTS5AuxiliaryFunctionAPI apiObj
        = FTS5AuxiliaryFunctionAPI(pApi, pFts, pCtx, apVal, nVal);
        AbstractFTS5AuxiliaryFunctionObject *funcObject = apiObj.getOrCreateFunctionObject(
        [&apiObj](void *userData) {
            AbstractFTS5AuxiliaryFunctionObject *funcObject
            = static_cast<AbstractFTS5AuxiliaryFunctionObject *>(
            new AuxiliaryFunctionObject(userData, apiObj));
            return funcObject;
        },
        deleteAuxData);

        if (funcObject != nullptr) {
            funcObject->process(apiObj);
        }
    }

    static void deleteAuxData(void *auxObject)
    {
        if (auxObject != nullptr) {
            delete static_cast<AuxiliaryFunctionObject *>(auxObject);
        }
    }
};

} // namespace WCDB
