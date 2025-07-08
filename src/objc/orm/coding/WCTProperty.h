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

#import "WCTColumnBinding.h"
#import "WCTCommon.h"

class WCDB_API WCTProperty final : public WCDB::Column, public WCTColumnBindingHolder {
public:
    WCTProperty();
    WCTProperty(const WCTColumnBinding& columnBinding);
    WCTProperty(const WCDB::UnsafeStringView& name, const WCTColumnBinding& columnBinding);
    ~WCTProperty() override;

    WCTProperty table(const WCDB::UnsafeStringView& table) const;
    WCTProperty schema(const WCDB::Schema& schema) const;

private:
    WCTProperty(const WCTColumnBinding& columnBinding, const Column& column);
    void configTableBindingRetrive();
};

namespace WCDB {

template<>
class WCDB_API SyntaxList<WCTProperty> final : public _SyntaxList<WCTProperty> {
public:
    using _SyntaxList<WCTProperty>::_SyntaxList;
    ~SyntaxList() override;

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

} // namespace WCDB
