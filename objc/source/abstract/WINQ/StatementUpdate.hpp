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

#ifndef StatementUpdate_hpp
#define StatementUpdate_hpp

#include <WCDB/Describable.hpp>
#include <WCDB/Statement.hpp>

class StatementUpdate : public DescribableWithLang<LangUpdateSTMT>,
                        public CRUDStatement {
public:
                            StatementUpdate& with(const WithClause& withClause);
                            StatementUpdate& update(const QualifiedTableName& qualifiedTableName);
                            StatementUpdate& updateOrRollback(const QualifiedTableName& qualifiedTableName);
                            StatementUpdate& updateOrAbort(const QualifiedTableName& qualifiedTableName);
                            StatementUpdate& updateOrReplace(const QualifiedTableName& qualifiedTableName);
                            StatementUpdate& updateOrFail(const QualifiedTableName& qualifiedTableName);
                            StatementUpdate& updateOrIgnore(const QualifiedTableName& qualifiedTableName);
                            
                            StatementUpdate& set(const std::string& columnName, const Expression& expression);
                            StatementUpdate& set(const std::list<std::string>& columnNames, const Expression& expression);
                            
                            StatementUpdate& where(const Expression& condition);
                            
                            StatementUpdate& orderBy(const OrderingTerm& orderingTerm);
                            StatementUpdate& orderBy(const std::list<OrderingTerm>& orderingTerms);
                            
                            StatementUpdate& limit(const Expression& from, const Expression& to);
                            StatementUpdate& limit(const Expression& limit);
                            StatementUpdate& offset(const Expression& offset);

                            
    virtual Type getType() const override;
                        protected:
                            void update(const QualifiedTableName& qualifiedTableName, const LangUpdateSTMT::Type& type);
};

#endif /* StatementUpdate_hpp */
