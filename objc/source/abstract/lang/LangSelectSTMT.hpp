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

#include <WCDB/LangCommon.hpp>

namespace WCDB {

namespace Lang {

class SelectSTMT : public CRUDSTMT {
public:
    SelectSTMT();

    bool recursive;
    CopyOnWriteLazyLangList<CommonTableExpression> commonTableExpressions;

    CopyOnWriteLazyLang<SelectCore> selectCore;

    class Compound : public Lang {
    public:
        Compound();

        enum class Operator : int {
            NotSet,
            Union,
            UnionAll,
            Intersect,
            Except,
        };
        Operator compoundOperator;
        CopyOnWriteLazyLang<SelectCore> selectCore;

        CopyOnWriteString SQL() const override;

    protected:
        static constexpr const char *
        OperatorName(const Operator &compoundOperator);
    };
    CopyOnWriteLazyLangList<Compound> compoundCores;

    CopyOnWriteLazyLangList<OrderingTerm> orderingTerms;

    CopyOnWriteLazyLang<Expr> limit;
    bool offset;
    CopyOnWriteLazyLang<Expr> limitParameter;

    CopyOnWriteString SQL() const override;

    STMT::Type getSTMTType() const override;
    static STMT::Type getType();
};

template <>
CopyOnWriteString
CopyOnWriteLazyLangList<SelectSTMT::Compound>::calculatedDescription() const;

} // namespace Lang

} // namespace WCDB

#endif /* LangSelectSTMT_hpp */
