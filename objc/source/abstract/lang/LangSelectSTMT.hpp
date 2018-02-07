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

#ifndef LangSelectSTMT_hpp
#define LangSelectSTMT_hpp

#include <WCDB/lang_common.h>

namespace WCDB {

namespace lang {

class SelectSTMT : public CRUDLang {
public:
    bool recursive;
    copy_on_write_lazy_lang_list<CommonTableExpression> commonTableExpressions;

    copy_on_write_lazy_lang<SelectCore> selectCore;

    class Compound : public Lang {
    public:
        enum class Operator : int {
            Union,
            UnionAll,
            Intersect,
            Except,
        };
        Operator compoundOperator;
        copy_on_write_lazy_lang<SelectCore> selectCore;

        virtual copy_on_write_string SQL() const override;

    protected:
        static constexpr const char *
        OperatorName(const Operator &compoundOperator);
    };
    copy_on_write_lazy_lang_list<Compound> compoundCores;

    copy_on_write_lazy_lang_list<OrderingTerm> orderingTerms;

    copy_on_write_lazy_lang<Expr> limit;
    bool offset;
    copy_on_write_lazy_lang<Expr> limitParameter;

    virtual copy_on_write_string SQL() const override;
};

} // namespace lang

} // namespace WCDB

#endif /* LangSelectSTMT_hpp */
