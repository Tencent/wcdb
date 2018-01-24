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

#ifndef column_hpp
#define column_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/declare.hpp>
#include <WCDB/operable.hpp>
#include <WCDB/redirectable.hpp>

namespace WCDB {

template <>
class Redirectable<Column> {
public:
    template <typename T>
    T as(const T &t) const;

private:
    virtual const std::string &getRedirectableDescription() const = 0;
};

class Column : public Describable,
               public Operable,
               public Redirectable<Column> {
public:
    static const Column All;
    static const Column Rowid;

    template <typename T>
    Column(const T &t,
           typename std::enable_if<ColumnConvertible<T>::value, void>::type * =
               nullptr)
        : Describable(ColumnConvertible<T>::asColumn(t).getDescription())
    {
    }

    Column(const std::string &name);

    Column inTable(const std::string &table) const;

    operator std::list<const Column>() const;

protected:
    virtual const std::string &getRedirectableDescription() const override;
    virtual Expression asExpression() const override;
};

} //namespace WCDB

#endif /* column_hpp */
