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

#ifndef CTETableName_hpp
#define CTETableName_hpp

#include <WCDB/Describable.hpp>

namespace WCDB {

class CTETableName : public DescribableWithLang<Lang::CTETableName> {
public:
    template <typename T, typename Enable = void>
    CTETableName(
        const T &t,
        typename std::enable_if<ColumnIsTextType<T>::value>::type * = nullptr)
    {
        setTableName(ColumnIsTextType<T>::asUnderlyingType(t));
    }

    CTETableName &byAddingColumn(const Column &column);

    CTETableName &byAddingColumns(const std::list<Column> &columns);

protected:
    void setTableName(const std::string &name);
};

} // namespace WCDB

#endif /* CTETableName_hpp */
