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

#ifndef Statement_hpp
#define Statement_hpp

#include <WCDB/SQL.hpp>

namespace WCDB {

class Statement : public SQL {
public:
    virtual ~Statement();
};

template<typename T>
class StatementSyntax : public Statement, public TypedSyntax<T> {
public:
    virtual ~StatementSyntax() {}

    Type getType() const override final
    {
        return TypedSyntax<T>::syntax.getType();
    }

    void iterate(const Iterator& iterator, void* parameter) override final
    {
        return TypedSyntax<T>::syntax.iterate(iterator, parameter);
    }

    std::string getDescription() const override final
    {
        return TypedSyntax<T>::syntax.getDescription();
    }
};

} // namespace WCDB

#endif /* Statement_hpp */
