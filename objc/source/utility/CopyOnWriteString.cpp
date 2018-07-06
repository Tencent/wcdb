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

#include <WCDB/CopyOnWriteString.hpp>
#include <WCDB/String.hpp>

CopyOnWriteString::CopyOnWriteString() : Super()
{
}

CopyOnWriteString::CopyOnWriteString(const std::string &string) : Super(string)
{
}

CopyOnWriteString::CopyOnWriteString(const char *string)
: Super(string ? string : WCDB::String::empty())
{
}

//void CopyOnWriteString::append(const CopyOnWriteString &string)
//{
//    if (!empty()) {
//        get_or_copy().append(string.get());
//    } else {
//        assign(string);
//    }
//}
//
//void CopyOnWriteString::append(const std::string &string)
//{
//    if (!empty()) {
//        get_or_copy().append(string);
//    } else {
//        assign(string);
//    }
//}

bool CopyOnWriteString::empty() const
{
    return CopyOnWrite<std::string>::empty() || CopyOnWrite<std::string>::get().empty();
}

bool CopyOnWriteString::isNull() const
{
    return CopyOnWrite<std::string>::empty();
}

bool CopyOnWriteString::equal(const std::string &other) const
{
    if (CopyOnWrite<std::string>::empty()) {
        return false;
    }
    return get() == other;
}

const std::string &CopyOnWriteString::get() const
{
    if (!empty()) {
        return CopyOnWrite<std::string>::get();
    }
    return WCDB::String::empty();
}
