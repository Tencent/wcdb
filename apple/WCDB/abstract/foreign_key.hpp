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

#ifndef foreign_key_hpp
#define foreign_key_hpp

#include <WCDB/declare.hpp>
#include <WCDB/describable.hpp>

namespace WCDB {

class ForeignKey : public Describable {
public:
    ForeignKey(const std::string &foreignTableName,
               const std::list<const std::string> &columnNames = {});

    enum class Action {
        SetNull,
        SetDefault,
        Cascade,
        Restrict,
        NoAction,
    };
    ForeignKey &onDelete(Action action);
    ForeignKey &onUpdate(Action action);
    ForeignKey &match(const std::string &name);

    enum class Deferrable {
        None,
        Deferred,
        Immediate,
    };
    ForeignKey &deferrable(Deferrable deferrable);
    ForeignKey &notDeferrable(Deferrable deferrable);

protected:
    static std::string actionName(Action action);
    static std::string deferrableName(Deferrable deferrable);
};

} //namespace WCDB

#endif /* foreign_key_hpp */
