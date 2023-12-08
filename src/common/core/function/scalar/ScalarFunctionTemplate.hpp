//
// Created by 陈秋文 on 2023/11/25.
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

#include "ScalarFunctionModule.hpp"

namespace WCDB {

template<typename ScalarFunctionObject>
class ScalarFunctionTemplate {
public:
    static_assert(std::is_base_of<AbstractScalarFunctionObject, ScalarFunctionObject>::value, "");

    static ScalarFunctionModule
    specialize(int parameterNum, bool deterministic = true, void* userContext = nullptr)
    {
        return ScalarFunctionModule(run, parameterNum, deterministic, userContext);
    }

    static void run(SQLiteContext* pCtx, int nVal, SQLiteValue** apVal)
    {
        ScalarFunctionAPI apiObj = ScalarFunctionAPI(pCtx, apVal, nVal);
        ScalarFunctionObject functionObject(apiObj.getUserData(), apiObj);
        functionObject.process(apiObj);
    }
};

} // namespace WCDB
