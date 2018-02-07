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

#ifndef JoinConstraint_hpp
#define JoinConstraint_hpp

#include <WCDB/Describable.hpp>

class JoinConstraint : public DescribableWithLang<LangJoinConstraint> {
public:
    JoinConstraint& on(const Expression& expression);
    JoinConstraint& usingColumn(const std::string columnName);
    JoinConstraint& usingColumns(const std::list<std::string> columnNames);
};

#endif /* JoinConstraint_hpp */
