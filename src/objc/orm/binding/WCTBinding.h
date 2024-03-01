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

#import "BaseBinding.hpp"
#import "CaseInsensitiveList.hpp"
#import "StringView.hpp"
#import "WCTColumnCoding.h"
#import "WCTCommon.h"
#import "WCTProperty.h"
#import "WCTTableCoding.h"
#import <map>

class WCDB_API WCTBinding final : public WCDB::BaseBinding {
#pragma mark - Binding
public:
    WCTBinding(Class cls);
    ~WCTBinding() override;
    static void assertNoInheritance(const WCTProperty &property, Class cls);

private:
    Class m_cls;

#pragma mark - Property
public:
    const WCTProperties &getProperties() const;

private:
    WCTProperties m_properties;

#pragma mark - Column Def
public:
    WCDB::ColumnDef *getColumnDef(const WCTProperty &property);
};
