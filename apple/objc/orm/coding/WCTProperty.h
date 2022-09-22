//
// Created by sanhuazhang on 2019/05/02
//

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
#import <WCDB/WCTCommon.h>

class WCTProperty final : public WCDB::Column, public WCTColumnBindingHolder {
public:
    WCTProperty();
    WCTProperty(const WCTColumnBinding& columnBinding);
    WCTProperty(const WCDB::UnsafeStringView& name, const WCTColumnBinding& columnBinding);
    ~WCTProperty() override final;

    WCDB::IndexedColumn asIndex() const;
    WCDB::IndexedColumn asIndex(const WCTOrder& order) const;

    WCDB::OrderingTerm asOrder() const;
    WCDB::OrderingTerm asOrder(const WCTOrder& order) const;

    WCDB::Column table(const WCDB::UnsafeStringView& table) const;
};

namespace WCDB {

template<>
class SyntaxList<WCTProperty> final : public _SyntaxList<WCTProperty> {
public:
    using _SyntaxList<WCTProperty>::_SyntaxList;
    ~SyntaxList() override final;

    WCTResultColumns redirect(const WCDB::ResultColumns& resultColumns) const;

    typedef WCDB::ResultColumn (^WCTRedirectBlock)(const WCTProperty&);
    WCTResultColumns redirect(WCTRedirectBlock block) const;

    // It's not the count of the list, but the SQL function `count(*)`.
    WCDB::Expression count() const;

    WCDB::Expressions table(NSString* table) const;

    WCTProperties propertiesByAddingNewProperties(const WCTProperties& properties) const;
    WCTProperties& addingNewProperties(const WCTProperties& properties);

    WCTProperties propertiesByRemovingProperties(const WCTProperties& properties) const;
    WCTProperties& removingProperties(const WCTProperties& properties);

private:
    static bool isEqual(const WCTProperty& left, const WCTProperty& right);
};

}
