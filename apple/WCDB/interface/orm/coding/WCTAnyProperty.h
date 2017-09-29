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

#import <Foundation/Foundation.h>
#import <WCDB/WCTDeclare.h>
#import <WCDB/abstract.h>

class WCTAnyProperty : public WCDB::Column {
public:
    WCTAnyProperty();

    WCTAnyProperty(Class cls);

    WCTAnyProperty inTable(NSString *tableName) const;

    //function
    WCTExpr avg(bool distinct = false) const;
    WCTExpr count(bool distinct = false) const;
    WCTExpr groupConcat(bool distinct = false) const;
    WCTExpr groupConcat(NSString *seperator, bool distinct = false) const;
    WCTExpr max(bool distinct = false) const;
    WCTExpr min(bool distinct = false) const;
    WCTExpr sum(bool distinct = false) const;
    WCTExpr total(bool distinct = false) const;

    Class getClass() const;

protected:
    Class m_cls;
    WCTAnyProperty(const Column &column, Class cls);
};
