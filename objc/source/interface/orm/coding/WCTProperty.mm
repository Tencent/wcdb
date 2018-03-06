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

#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTProperty.h>

WCTProperty::WCTProperty(const std::shared_ptr<WCTColumnBinding> &columnBinding)
    : m_columnBinding(columnBinding)
{
    assert(columnBinding != nullptr);
    WCDB::Lang::Expr &lang = getMutableLang();
    lang.type = WCDB::Lang::Expr::Type::Column;
    lang.exprColumn.get_or_copy().column.assign(m_columnBinding->columnDef.getLang().get().column);
}

WCTProperty::WCTProperty(const WCDB::Expression &expression,
                         const std::shared_ptr<WCTColumnBinding> &columnBinding)
    : WCDB::DescribableWithLang<WCDB::Lang::Expr>(expression.getLang())
    , m_columnBinding(columnBinding)
{
    assert(columnBinding != nullptr);
}

WCTProperty::operator WCDB::Column() const
{
    return m_columnBinding->columnDef.getLang().get().column;
}

WCTProperty::operator WCDB::Expression() const
{
    return getLang();
}

WCTProperty::operator WCDB::ResultColumn() const
{
    return operator WCDB::Expression();
}

WCTProperty::operator WCDB::OrderingTerm() const
{
    return operator WCDB::Expression();
}

WCTProperty::operator WCDB::IndexedColumn() const
{
    return operator WCDB::Expression();
}

WCTProperty WCTProperty::atTable(NSString *tableName) const
{
    WCDB::Expression expression(getLang());
    expression.withTable(tableName.UTF8String);
    return WCTProperty(expression, m_columnBinding);
}

WCTProperty WCTProperty::atTable(const std::string &tableName) const
{
    WCDB::Expression expression(getLang());
    expression.withTable(tableName);
    return WCTProperty(expression, m_columnBinding);
}

WCTProperty WCTProperty::atSchema(NSString *schemaName) const
{
    WCDB::Expression expression(getLang());
    expression.withSchema(schemaName.UTF8String);
    return WCTProperty(expression, m_columnBinding);
}

WCTProperty WCTProperty::atSchema(const std::string &schemaName) const
{
    WCDB::Expression expression(getLang());
    expression.withSchema(schemaName);
    return WCTProperty(expression, m_columnBinding);
}

const std::shared_ptr<WCTColumnBinding> &WCTProperty::getColumnBinding() const
{
    return m_columnBinding;
}

WCDB::IndexedColumn WCTProperty::asIndex(WCDB::Order order) const
{
    return WCDB::IndexedColumn(WCDB::Expression(getLang())).withOrder(order);
}

WCDB::OrderingTerm WCTProperty::asOrder(WCDB::Order order) const
{
    return WCDB::OrderingTerm(getLang()).withOrder(order);
}

const WCDB::ColumnDef &WCTProperty::getColumnDef() const
{
    return m_columnBinding->columnDef;
}

WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr> WCTProperty::getExpressionLang() const
{
    return getLang();
}

WCDB::Expression WCTProperty::getRedirectSource() const
{
    return getLang();
}

WCTPropertyList::WCTPropertyList()
    : std::list<WCTProperty>()
{
}

WCTPropertyList::WCTPropertyList(const WCTProperty &property)
    : std::list<WCTProperty>({property})
{
}

WCTPropertyList::WCTPropertyList(const std::initializer_list<WCTProperty> &properties)
    : std::list<WCTProperty>(properties)
{
}

WCTPropertyList::WCTPropertyList(const std::list<WCTProperty> &properties)
    : std::list<WCTProperty>(properties)
{
}

WCTPropertyList::operator std::list<WCDB::Column>() const
{
    return std::list<WCDB::Column>(begin(), end());
    ;
}

WCTPropertyList::operator std::list<WCDB::Expression>() const
{
    return std::list<WCDB::Expression>(begin(), end());
}

WCTPropertyList::operator std::list<WCDB::ResultColumn>() const
{
    return std::list<WCDB::ResultColumn>(begin(), end());
}

WCTPropertyList::operator std::list<WCDB::OrderingTerm>() const
{
    return std::list<WCDB::OrderingTerm>(begin(), end());
}

WCTPropertyList::operator std::list<WCDB::IndexedColumn>() const
{
    return std::list<WCDB::IndexedColumn>(begin(), end());
}

namespace WCDB {

Expression ExpressionConvertible<WCTProperty>::as(const WCTProperty &property)
{
    return Expression(property.getLang());
}

} //namespace WCDB
