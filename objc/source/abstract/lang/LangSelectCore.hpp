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

#ifndef LangSelectCore_hpp
#define LangSelectCore_hpp

#include <WCDB/lang_common.h>

class LangSelectCore : public Lang {
public:
    enum class Switch : int {
        Select,
        Values,
    };
    Switch switcher;

    bool distinct;
    copy_on_write_lazy_lang_list<LangResultColumn> resultColumns;

    enum class FromSwitch : int {
        NotSet,
        TableOrSubquery,
        JoinClause,
    };
    FromSwitch fromSwitcher;
    copy_on_write_lazy_lang_list<LangTableOrSubquery> tableOrSubquerys;
    copy_on_write_lazy_lang<LangJoinClause> joinClause;

    copy_on_write_lazy_lang<LangExpr> condition;

    copy_on_write_lazy_lang_list<LangExpr> groups;

    copy_on_write_lazy_lang<LangExpr> having;

    copy_on_write_lazy_lang_list<LangExpr> values;

    virtual copy_on_write_string SQL() const override;
};

#endif /* LangSelectCore_hpp */
