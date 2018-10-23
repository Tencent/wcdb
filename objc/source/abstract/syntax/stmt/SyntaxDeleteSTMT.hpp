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

#ifndef SyntaxDeleteSTMT_hpp
#define SyntaxDeleteSTMT_hpp

#include <WCDB/SyntaxIdentifier.hpp>

namespace WCDB {

namespace Syntax {

class DeleteSTMT : public Identifier {
#pragma mark - Lang
public:
    bool useWithClause = false;
    WithClause withClause;
    QualifiedTableName table;
    bool useCondition = false;
    Expression condition;
    std::list<OrderingTerm> orderingTerms;
    bool useLimit = false;
    Expression limit;
    enum class LimitParameterType {
        NotSet,
        Offset,
        End,
    } limitParameterType
    = LimitParameterType::NotSet;
    Expression limitParameter;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::DeleteSTMT;
    Type getType() const override final;
    std::string getDescription() const override final;
    void iterate(const Iterator& iterator, void* parameter) override final;
};

} // namespace Syntax

} // namespace WCDB

#endif /* SyntaxDeleteSTMT_hpp */
