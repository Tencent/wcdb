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

#import <WCDB/Assertion.hpp>
#import <WCDB/Core.h>
#import <WCDB/Error.hpp>
#import <WCDB/Notifier.hpp>
#import <WCDB/StringView.hpp>
#import <WCDB/WCTRuntimeObjCAccessor.h>

WCTRuntimeObjCAccessor::WCTRuntimeObjCAccessor(Class instanceClass, const WCDB::UnsafeStringView &propertyName)
: WCTRuntimeAccessor<id>(instanceClass, propertyName)
, WCTObjCAccessor()
, m_propertyClass(getPropertyClass(instanceClass, propertyName))
, m_impForArchiveSelector(getInstanceMethodImplementation(m_propertyClass, archiveSelector()))
, m_impForUnarchiveSelector(getClassMethodImplementation(m_propertyClass, unarchiveSelector()))
, m_columnType(GetColumnType(m_propertyClass))
{
    if (WCDB::Console::debuggable()) {
        WCTAssert(m_propertyClass != nil, WCDB::StringView::formatted("Unable to find out the %s.%s.", NSStringFromClass(instanceClass).UTF8String, propertyName.data()));
        WCTAssert([m_propertyClass conformsToProtocol:@protocol(WCTColumnCoding)], WCDB::StringView::formatted("%s should conform to protocol WCTColumnCoding.", propertyName.data()));
    }
}

WCTRuntimeObjCAccessor::~WCTRuntimeObjCAccessor()
{
}

void WCTRuntimeObjCAccessor::setObject(InstanceType instance, OCType value)
{
    using Unarchiver = PropertyType (*)(Class, SEL, OCType);
    if (instance != nil) {
        PropertyType property = ((Unarchiver) m_impForUnarchiveSelector)(m_propertyClass, unarchiveSelector(), value);
        setProperty(instance, property);
    }
}

WCTRuntimeObjCAccessor::OCType WCTRuntimeObjCAccessor::getObject(InstanceType instance)
{
    using Archiver = OCType (*)(InstanceType, SEL);
    PropertyType property = getProperty(instance);
    OCType value = property ? ((Archiver) m_impForArchiveSelector)(property, archiveSelector()) : nil;
    return value;
}

WCDB::ColumnType WCTRuntimeObjCAccessor::getColumnType() const
{
    return m_columnType;
}

SEL WCTRuntimeObjCAccessor::archiveSelector()
{
    static SEL s_archiveSelector = NSSelectorFromString(@"archivedWCTValue");
    return s_archiveSelector;
}

SEL WCTRuntimeObjCAccessor::unarchiveSelector()
{
    static SEL s_unarchiveSelector = NSSelectorFromString(@"unarchiveWithWCTValue:");
    return s_unarchiveSelector;
}

WCDB::ColumnType WCTRuntimeObjCAccessor::GetColumnType(Class propertyClass)
{
    static const SEL s_columnTypeSelector = NSSelectorFromString(@"columnType");
    IMP implementation = getClassMethodImplementation(propertyClass, s_columnTypeSelector);
    using GetColumnTyper = WCDB::ColumnType (*)(Class, SEL);
    return ((GetColumnTyper) implementation)(propertyClass, s_columnTypeSelector);
}
