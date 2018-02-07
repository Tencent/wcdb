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

#ifndef LangJoinClause_hpp
#define LangJoinClause_hpp

#include <WCDB/lang_common.h>

namespace WCDB {

namespace lang {

class JoinClause : public Lang {
public:
    copy_on_write_lazy_lang<TableOrSubquery> tableOrSubquery;

    class Operand : public Lang {
    public:
        copy_on_write_lazy_lang<JoinOperator> joinOperator;
        copy_on_write_lazy_lang<TableOrSubquery> tableOrSubquery;
        copy_on_write_lazy_lang<JoinConstraint> joinConstraint;

        virtual copy_on_write_string SQL() const override;
    };
    copy_on_write_lazy_lang_list<Operand> joinOperands;

    virtual copy_on_write_string SQL() const override;
};

template <>
copy_on_write_string
copy_on_write_lazy_lang_list<JoinClause::Operand>::calculatedDescription()
    const;

} // namespace lang

} // namespace WCDB

#endif /* LangJoinClause_hpp */
