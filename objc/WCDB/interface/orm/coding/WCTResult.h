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
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTPropertyBase.h>

class WCTResult : public WCDB::ColumnResult, public WCTPropertyBase {
public:
    WCTResult(const WCTExpr &expr);
    WCTResult(const WCTProperty &property);
    WCTResult &as(const WCTProperty &property);

    WCTResultList distinct() const;

    NSString *getDescription() const;
};

class WCTResultList : public std::list<const WCTResult> {
public:
    WCTResultList();

    template <typename T = WCTResult>
    WCTResultList(
        const T &value,
        typename std::enable_if<
            std::is_constructible<WCTResult, T>::value>::type * = nullptr)
        : std::list<const WCTResult>({WCTResult(value)}), m_distinct(false)
    {
    }

    WCTResultList(std::initializer_list<const WCTExpr> il);
    WCTResultList(std::initializer_list<const WCTProperty> il);

    WCTResultList(const WCTPropertyList &propertyList);
    WCTResultList(const WCTExprList &exprList);

    WCTResultList(std::initializer_list<const WCTPropertyList> il);

    WCTResultList &distinct();

    bool isDistinct() const;

protected:
    bool m_distinct;
};
