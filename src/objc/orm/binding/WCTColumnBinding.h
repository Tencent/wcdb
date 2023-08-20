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

#import "WCTCommon.h"
#import "WCTRuntimeCppAccessor.h"
#import "WCTRuntimeObjCAccessor.h"

class WCDB_API WCTColumnBinding final {
public:
    template<typename T>
    static WCTColumnBinding generate(Class cls, const WCDB::UnsafeStringView& propertyName)
    {
        return WCTColumnBinding(cls, propertyName, (T*) nullptr);
    }

    template<typename T>
    WCTColumnBinding(Class cls,
                     const WCDB::UnsafeStringView& propertyName,
                     T* = nullptr,
                     typename std::enable_if<!WCDB::IsObjCType<T>::value>::type* = nullptr)
    : WCTColumnBinding(cls,
                       std::static_pointer_cast<WCTBaseAccessor>(
                       std::make_shared<WCTRuntimeCppAccessor<T>>(cls, propertyName)),
                       propertyName)
    {
    }

    template<typename T>
    WCTColumnBinding(Class cls,
                     const WCDB::UnsafeStringView& propertyName,
                     T* = nullptr,
                     typename std::enable_if<WCDB::IsObjCType<T>::value>::type* = nullptr)
    : WCTColumnBinding(cls,
                       std::static_pointer_cast<WCTBaseAccessor>(
                       std::make_shared<WCTRuntimeObjCAccessor>(cls, propertyName, [std::remove_pointer_t<WCDB::remove_ownership_t<T>> class])),
                       propertyName)
    {
    }

    WCTColumnBinding();

    Class getClass() const;
    const WCDB::UnsafeStringView& getPropertyName() const;
    const std::shared_ptr<const WCTBaseAccessor>& getAccessor() const;

    bool operator==(const WCTColumnBinding& other) const;

protected:
    WCTColumnBinding(Class cls,
                     const std::shared_ptr<const WCTBaseAccessor> accessor,
                     const WCDB::UnsafeStringView& propertyName);

    Class m_class;
    WCDB::StringView m_propertyName;
    std::shared_ptr<const WCTBaseAccessor> m_accessor;
};

class WCDB_API WCTColumnBindingHolder {
public:
    WCTColumnBindingHolder();
    WCTColumnBindingHolder(const WCTColumnBinding& columnBinding);
    virtual ~WCTColumnBindingHolder() = 0;

    WCTResultColumn redirect(const WCDB::ResultColumn& resultColumn) const;

    const WCTColumnBinding& getColumnBinding() const;

protected:
    WCTColumnBinding m_columnBinding;
};
