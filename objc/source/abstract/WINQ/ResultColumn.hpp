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

#ifndef ResultColumn_hpp
#define ResultColumn_hpp

#include <WCDB/Describable.hpp>
#include <WCDB/Redirectable.hpp>

namespace WCDB {

class ResultColumn : public DescribableWithLang<lang::ResultColumn> {
public:
    static const ResultColumn All;

    template <typename T>
    ResultColumn(const T &t,
                 typename std::enable_if<ExpressionConvertible<T>::value>::type
                     * = nullptr)
    {
        setupWithExpression(ExpressionConvertible<T>::as(t));
    }

    ResultColumn(const Expression &expression);
    ResultColumn(const Expression::All &all);

    ResultColumn &as(const Column &columnAlias);
    ResultColumn &withTable(const std::string &tableName);

protected:
    ResultColumn(const lang::ResultColumn::Type &type);
    void setupWithExpression(const Expression &expression);
    void setType(const lang::ResultColumn::Type &type);
};

} // namespace WCDB

#endif /* ResultColumn_hpp */
