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

#ifndef LangRaiseFunction_hpp
#define LangRaiseFunction_hpp

#include <WCDB/LangCommon.h>

namespace WCDB {

namespace Lang {

class RaiseFunction : public Lang {
public:
    RaiseFunction();
    enum class Type : int {
        Ignore,
        Rollback,
        Abort,
        Fail,
    };
    Type type;
    CopyOnWriteString errorMessage;

    CopyOnWriteString SQL() const override;
};

} // namespace Lang

} // namespace WCDB

#endif /* LangRaiseFunction_hpp */
