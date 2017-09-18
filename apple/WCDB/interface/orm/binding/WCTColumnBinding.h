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
#import <WCDB/WCTRuntimeCppAccessor.h>
#import <WCDB/WCTRuntimeObjCAccessor.h>
#import <WCDB/abstract.h>

class WCTColumnBinding {
public:
    template <typename T>
    WCTColumnBinding(
        Class cls,
        const std::string &pn,
        const std::string &cn,
        T * = nullptr,
        typename std::enable_if<ColumnIsCppType<T>::value>::type * = nullptr)
        : columnName(cn)
        , accessor(new WCTRuntimeCppAccessor<T>(cls, pn))
        , m_columnDef(cn, (WCDB::ColumnType) accessor->getColumnType())
        , m_isAutoIncrement(false)
        , m_isPrimary(false)
    {
    }

    template <typename T>
    WCTColumnBinding(
        Class cls,
        const std::string &pn,
        const std::string &cn,
        T * = nullptr,
        typename std::enable_if<ColumnIsObjCType<T>::value>::type * = nullptr)
        : columnName(cn)
        , accessor(new WCTRuntimeObjCAccessor(cls, pn))
        , m_columnDef(cn, (WCDB::ColumnType) accessor->getColumnType())
        , m_isAutoIncrement(false)
        , m_isPrimary(false)
    {
    }

    const std::string columnName;
    const std::shared_ptr<WCTBaseAccessor> accessor;
    bool isPrimary() const;
    bool isAutoIncrement() const;
    const WCTColumnDef getColumnDef() const;

    void
    makePrimary(WCTOrderTerm order, bool isAutoIncrement, WCTConflict conflict);

    void makeNotNull();

    void makeUnique();

    template <typename T>
    typename std::enable_if<ColumnIsObjCType<T>::value, void>::type
    makeDefault(WCTValue *defaultValue)
    {
        makeDefaultObjC(defaultValue);
    }

    template <typename T>
    typename std::enable_if<ColumnIsCppType<T>::value, void>::type
    makeDefault(const typename WCDB::ColumnInfo<T>::CType &defaultValue)
    {
        m_columnDef.makeDefault(defaultValue);
    }

    template <typename T>
    void makeDefault(WCTDefaultType defaultType)
    {
        m_columnDef.makeDefault((WCDB::ColumnDef::DefaultType) defaultType);
    }

protected:
    void makeDefaultObjC(WCTValue *defaultValue);
    bool m_isPrimary;
    bool m_isAutoIncrement;
    WCTColumnDef m_columnDef;
};
