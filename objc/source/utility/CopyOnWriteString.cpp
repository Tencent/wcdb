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

CopyOnWriteString::CopyOnWriteString() : Super()
{
}

CopyOnWriteString::CopyOnWriteString(const std::string &string) : Super(string)
{
}

CopyOnWriteString::CopyOnWriteString(const char *string)
    : Super(string ? string : "")
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
    return CopyOnWrite<std::string>::empty() || get().empty();
}
