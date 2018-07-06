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

#import <WCDB/NSString+CppString.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTProperty.h>

WCTProperty::WCTProperty(const WCTColumnBinding &columnBinding)
: m_columnBinding(&columnBinding)
{
    WCDB::Lang::Expr &lang = getMutableLang();
    lang.type = WCDB::Lang::ExprBase::Type::Column;
    lang.exprColumn.get_or_copy().column.assign(m_columnBinding->columnDef.getCOWLang().get<WCDB::Lang::ColumnDef>().column);
}

WCTProperty::WCTProperty(const WCDB::Expression &expression,
                         const WCTColumnBinding &columnBinding)
: WCDB::DescribableWithLang<WCDB::Lang::Expr>(expression.getCOWLang())
, m_columnBinding(&columnBinding)
{
}

WCTProperty::operator WCDB::Column() const
{
    return m_columnBinding->columnDef.getCOWLang().get<WCDB::Lang::ColumnDef>().column;
}

WCTProperty::operator WCDB::Expression() const
{
    return WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr>(getCOWLang());
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

WCTProperty::operator std::list<WCDB::ResultColumn>() const
{
    return operator WCDB::ResultColumn();
}

WCTProperty::operator std::list<WCDB::OrderingTerm>() const
{
    return operator WCDB::OrderingTerm();
}

WCTProperty WCTProperty::inTable(NSString *tableName) const
{
    WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr> cowLang(getCOWLang());
    WCDB::Expression expression(cowLang);
    expression.withTable(tableName.cppString);
    return WCTProperty(expression, *m_columnBinding);
}

WCTProperty WCTProperty::inSchema(NSString *schemaName) const
{
    WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr> cowLang(getCOWLang());
    WCDB::Expression expression(cowLang);
    expression.withSchema(schemaName.cppString);
    return WCTProperty(expression, *m_columnBinding);
}

const WCTColumnBinding &WCTProperty::getColumnBinding() const
{
    return *m_columnBinding;
}

bool WCTProperty::isSameColumnBinding(const WCTProperty &property) const
{
    return m_columnBinding == property.m_columnBinding;
}

WCDB::IndexedColumn WCTProperty::asIndex(WCDB::Order order) const
{
    return WCDB::IndexedColumn(WCDB::Expression(WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr>(getCOWLang()))).withOrder(order);
}

WCDB::OrderingTerm WCTProperty::asOrder(WCDB::Order order) const
{
    return WCDB::OrderingTerm(WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr>(getCOWLang())).withOrder(order);
}

const WCDB::ColumnDef &WCTProperty::getColumnDef() const
{
    return m_columnBinding->columnDef;
}

WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr> WCTProperty::getExpressionLang() const
{
    return getCOWLang();
}

WCDB::Expression WCTProperty::getRedirectSource() const
{
    return WCDB::Lang::CopyOnWriteLazyLang<WCDB::Lang::Expr>(getCOWLang());
}

WCTPropertyList::WCTPropertyList(const WCTProperty &property)
: std::list<WCTProperty>({ property })
{
}

WCTPropertyList WCTPropertyList::inTable(NSString *tableName) const
{
    WCTPropertyList properties;
    for (const WCTProperty &property : *this) {
        properties.push_back(property.inTable(tableName));
    }
    return properties;
}

WCTPropertyList WCTPropertyList::inSchema(NSString *schemaName) const
{
    WCTPropertyList properties;
    for (const WCTProperty &property : *this) {
        properties.push_back(property.inSchema(schemaName));
    }
    return properties;
}

void WCTPropertyList::addProperties(const WCTPropertyList &properties)
{
    for (const WCTProperty &property : properties) {
        push_back(property);
    }
}

WCTPropertyList WCTPropertyList::byAddingProperties(const WCTPropertyList &properties) const
{
    WCTPropertyList newProperties = *this;
    newProperties.addProperties(properties);
    return newProperties;
}

WCTPropertyList::operator std::list<WCDB::Column>() const
{
    return std::list<WCDB::Column>(begin(), end());
}

WCTPropertyList::operator std::list<WCDB::Expression>() const
{
    std::list<WCDB::Expression> expressions;
    for (const auto &property : *this) {
        expressions.push_back(property.operator WCDB::Expression());
    }
    return expressions;
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
