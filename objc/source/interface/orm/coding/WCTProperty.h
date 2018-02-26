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

#import <WCDB/WCTDeclare.h>
#import <WCDB/abstract.h>

class WCTColumnBinding;
class WCTProperty;

class WCTProperty : public WCDB::Operable,
                    public WCDB::DescribableWithLang<WCDB::lang::Expr>,
                    public WCDB::Redirectable {
public:
    WCTProperty(const std::shared_ptr<WCTColumnBinding> &columnBinding);
    WCTProperty(const WCDB::Expression &expression,
                const std::shared_ptr<WCTColumnBinding> &columnBinding);

    WCTProperty atTable(NSString *tableName) const;
    WCTProperty atTable(const std::string &tableName) const;
    WCTProperty atSchema(NSString *schemaName) const;
    WCTProperty atSchema(const std::string &schemaName) const;

    WCDB::IndexedColumn asIndex(WCDB::Order order = WCTOrderedNotSet) const;
    WCDB::OrderingTerm asOrder(WCDB::Order order = WCTOrderedNotSet) const;

    const WCDB::ColumnDef &getColumnDef() const;

    const std::shared_ptr<WCTColumnBinding> &getColumnBinding() const;

    operator WCDB::Column() const;
    operator WCDB::Expression() const;
    operator WCDB::ResultColumn() const;
    operator WCDB::OrderingTerm() const;
    operator WCDB::IndexedColumn() const;

protected:
    virtual WCDB::Expression getRedirectSource() const override;

    virtual WCDB::lang::copy_on_write_lazy_lang<WCDB::lang::Expr>
    getExpressionLang() const override;

    std::shared_ptr<WCTColumnBinding> m_columnBinding;
};

class WCTPropertyList : public std::list<WCTProperty> {
public:
    WCTPropertyList();
    WCTPropertyList(const WCTProperty &property);
    WCTPropertyList(const std::initializer_list<WCTProperty> &properties);
    WCTPropertyList(const std::list<WCTProperty> &properties);

    operator std::list<WCDB::Column>() const;
    operator std::list<WCDB::Expression>() const;
    operator std::list<WCDB::ResultColumn>() const;
    operator std::list<WCDB::OrderingTerm>() const;
    operator std::list<WCDB::IndexedColumn>() const;
};

namespace WCDB {

template <>
class ExpressionConvertible<WCTProperty> : public std::true_type {
public:
    static Expression as(const WCTProperty &property);
};

} //namespace WCDB
