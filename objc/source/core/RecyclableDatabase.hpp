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

#ifndef _WCDB_RECYCLABLEDATABASE_HPP
#define _WCDB_RECYCLABLEDATABASE_HPP

#include <WCDB/Database.hpp>
#include <WCDB/Recyclable.hpp>

namespace WCDB {

class RecyclableDatabase final : public Recyclable<Database *> {
public:
    using Super = Recyclable<Database *>;

    RecyclableDatabase();
    RecyclableDatabase(Database *value, const Super::OnRecycled &onRecycled);
    RecyclableDatabase(const std::nullptr_t &);

    bool operator==(const std::nullptr_t &) const;
    bool operator!=(const std::nullptr_t &) const;

    constexpr Database *operator->() const { return m_value; }
    Database *get() const;

protected:
    using Recyclable::unsafeGet;
};

} //namespace WCDB

#endif /* _WCDB_RECYCLABLEDATABASE_HPP */
