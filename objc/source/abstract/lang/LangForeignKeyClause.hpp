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

#ifndef LangForeignKeyClause_hpp
#define LangForeignKeyClause_hpp

#include <WINQ/lang_common.h>

namespace WCDB {

namespace lang {

class ForeignKeyClause : public Lang {
public:
    ForeignKeyClause();
    copy_on_write_string foreignTable;
    copy_on_write_lazy_lang_list<Column> columns;

    class Trigger : public Lang {
    public:
        Trigger();
        bool match;
        bool update;
        enum class Operation : int {
            NotSet,
            SetNull,
            SetDefault,
            Cascade,
            Restrict,
            NoAction,
        };
        Operation operation;
        copy_on_write_string name;

        virtual copy_on_write_string SQL() const override;

    protected:
        static constexpr const char *OperationName(const Operation &operation);
    };

    copy_on_write_lazy_lang_list<Trigger> triggers;

    bool doDeferrable;
    bool notDeferrable;
    enum class InitiallySwitch : int {
        NotSet,
        Deferred,
        Immediate,
    };
    InitiallySwitch initiallySwitcher;

    virtual copy_on_write_string SQL() const override;

protected:
    static constexpr const char *
    InitiallySwitchName(const InitiallySwitch &initiallySwitcher);
};

template <>
copy_on_write_string
copy_on_write_lazy_lang_list<ForeignKeyClause::Trigger>::calculatedDescription()
    const;

} // namespace lang

} // namespace WCDB

#endif /* LangForeignKeyClause_hpp */
