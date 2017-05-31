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
#import <WCDB/abstract.h>
#import <string>

typedef NS_ENUM(int, WCTConstraintBindingType) {
    WCTConstraintBindingPrimaryKey,
    WCTConstraintBindingUnique,
    WCTConstraintBindingCheck,
};

class WCTConstraintBindingBase {
public:
    WCTConstraintBindingBase(const std::string &name,
                             WCTConstraintBindingType type);
    const std::string name;
    const WCTConstraintBindingType type;

    virtual WCDB::TableConstraint generateConstraint() const = 0;
};

class WCTConstraintPrimaryKeyBinding : public WCTConstraintBindingBase {
public:
    static constexpr const WCTConstraintBindingType type =
        WCTConstraintBindingPrimaryKey;
    WCTConstraintPrimaryKeyBinding(const std::string &name);

    void addPrimaryKey(const WCTIndex &index);
    void setConflict(WCTConflict conflict);

    virtual WCDB::TableConstraint generateConstraint() const override;

protected:
    WCTConflict m_conflict;
    WCTIndexList m_primaryKeyList;
};

class WCTConstraintUniqueBinding : public WCTConstraintBindingBase {
public:
    static constexpr const WCTConstraintBindingType type =
        WCTConstraintBindingUnique;
    WCTConstraintUniqueBinding(const std::string &name);

    void addUnique(const WCTIndex &index);
    void setConflict(WCTConflict conflict);

    virtual WCDB::TableConstraint generateConstraint() const override;

protected:
    WCTConflict m_conflict;
    WCTIndexList m_uniqueList;
};

class WCTConstraintCheckBinding : public WCTConstraintBindingBase {
public:
    static constexpr const WCTConstraintBindingType type =
        WCTConstraintBindingCheck;
    WCTConstraintCheckBinding(const std::string &name);

    void makeCheck(const WCTExpr &expr);
    virtual WCDB::TableConstraint generateConstraint() const override;

protected:
    WCTExpr m_check;
};
