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

#import <WCDB/WCTCommon.h>
#import <WCDB/WCTRuntimeCppAccessor.h>
#import <WCDB/WCTRuntimeObjCAccessor.h>

class WCTColumnBinding final {
public:
    template<typename T>
    static WCTColumnBinding generate(Class cls, const WCDB::String& propertyName)
    {
        return WCTColumnBinding(cls, propertyName, (T*) nullptr);
    }

    template<typename T>
    WCTColumnBinding(Class cls,
                     const WCDB::String& propertyName,
                     T* = nullptr,
                     typename std::enable_if<!WCDB::IsObjCType<T>::value>::type* = nullptr)
    : WCTColumnBinding(
    cls, std::shared_ptr<WCTBaseAccessor>(new WCTRuntimeCppAccessor<T>(cls, propertyName)), propertyName)
    {
    }

    template<typename T>
    WCTColumnBinding(Class cls,
                     const WCDB::String& propertyName,
                     T* = nullptr,
                     typename std::enable_if<WCDB::IsObjCType<T>::value>::type* = nullptr)
    : WCTColumnBinding(
    cls, std::shared_ptr<WCTBaseAccessor>(new WCTRuntimeObjCAccessor(cls, propertyName)), propertyName)
    {
    }

    WCTColumnBinding();

    Class getClass() const;
    const WCDB::String& getPropertyName() const;
    const std::shared_ptr<WCTBaseAccessor>& getAccessor() const;

    bool operator==(const WCTColumnBinding& other) const;

protected:
    WCTColumnBinding(Class cls,
                     const std::shared_ptr<WCTBaseAccessor> accessor,
                     const WCDB::String& propertyName);

    std::shared_ptr<WCTBaseAccessor> m_accessor;
    WCDB::String m_propertyName;
    Class m_class;
};

class WCTColumnBindingHolder {
public:
    WCTColumnBindingHolder();
    WCTColumnBindingHolder(const WCTColumnBinding& columnBinding);
    virtual ~WCTColumnBindingHolder() = 0;

    WCTResultColumn redirect(const WCDB::ResultColumn& resultColumn) const;

    const WCTColumnBinding& getColumnBinding() const;

protected:
    WCTColumnBinding m_columnBinding;
};
