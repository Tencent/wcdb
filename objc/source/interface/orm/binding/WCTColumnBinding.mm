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
#import <WCDB/WCTResultColumn.h>

Class WCTColumnBinding::getClass() const
{
    return m_class;
}

WCTColumnBindingHolder::WCTColumnBindingHolder()
: m_columnBinding(nullptr)
{
}

WCTColumnBindingHolder::WCTColumnBindingHolder(const WCTColumnBinding& columnBinding)
: m_columnBinding(&columnBinding)
{
}

WCTColumnBindingHolder::~WCTColumnBindingHolder()
{
}

const WCTColumnBinding& WCTColumnBindingHolder::getColumnBinding() const
{
    return *m_columnBinding;
}

WCTResultColumn WCTColumnBindingHolder::redirect(const WCDB::ResultColumn& resultColumn) const
{
    return WCTResultColumn(resultColumn, *m_columnBinding);
}
