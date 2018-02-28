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

#ifndef JoinClause_hpp
#define JoinClause_hpp

#include <WCDB/Describable.hpp>

namespace WCDB {

class JoinClause : public DescribableWithLang<Lang::JoinClause> {
public:
    JoinClause(const TableOrSubquery &tableOrSubquery);

    JoinClause &with(const TableOrSubquery &tableOrSubquery,
                     const JoinConstraint &joinConstraint);
    JoinClause &join(const TableOrSubquery &tableOrSubquery,
                     const JoinConstraint &joinConstraint);

    JoinClause &leftJoin(const TableOrSubquery &tableOrSubquery,
                         const JoinConstraint &joinConstraint);
    JoinClause &leftOuterJoin(const TableOrSubquery &tableOrSubquery,
                              const JoinConstraint &joinConstraint);
    JoinClause &innerJoin(const TableOrSubquery &tableOrSubquery,
                          const JoinConstraint &joinConstraint);
    JoinClause &crossJoin(const TableOrSubquery &tableOrSubquery,
                          const JoinConstraint &joinConstraint);

    JoinClause &naturalJoin(const TableOrSubquery &tableOrSubquery,
                            const JoinConstraint &joinConstraint);
    JoinClause &naturalLeftJoin(const TableOrSubquery &tableOrSubquery,
                                const JoinConstraint &joinConstraint);
    JoinClause &naturalLeftOuterJoin(const TableOrSubquery &tableOrSubquery,
                                     const JoinConstraint &joinConstraint);
    JoinClause &naturalInnerJoin(const TableOrSubquery &tableOrSubquery,
                                 const JoinConstraint &joinConstraint);
    JoinClause &naturalCrossJoin(const TableOrSubquery &tableOrSubquery,
                                 const JoinConstraint &joinConstraint);

    JoinClause &with(const TableOrSubquery &tableOrSubquery);
    JoinClause &join(const TableOrSubquery &tableOrSubquery);

    JoinClause &leftJoin(const TableOrSubquery &tableOrSubquery);
    JoinClause &leftOuterJoin(const TableOrSubquery &tableOrSubquery);
    JoinClause &innerJoin(const TableOrSubquery &tableOrSubquery);
    JoinClause &crossJoin(const TableOrSubquery &tableOrSubquery);

    JoinClause &naturalJoin(const TableOrSubquery &tableOrSubquery);
    JoinClause &naturalLeftJoin(const TableOrSubquery &tableOrSubquery);
    JoinClause &naturalLeftOuterJoin(const TableOrSubquery &tableOrSubquery);
    JoinClause &naturalInnerJoin(const TableOrSubquery &tableOrSubquery);
    JoinClause &naturalCrossJoin(const TableOrSubquery &tableOrSubquery);

protected:
    void appendJoinOperand(bool natural,
                           const Lang::JoinOperator::Type &type,
                           const TableOrSubquery &tableOrSubquery,
                           const JoinConstraint &joinConstraint);

    void appendJoinOperand(bool natural,
                           const Lang::JoinOperator::Type &type,
                           const TableOrSubquery &tableOrSubquery);
};

} // namespace WCDB

#endif /* JoinClause_hpp */
