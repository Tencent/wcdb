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

#ifndef BindParameter_hpp
#define BindParameter_hpp

#include <WCDB/SQL.hpp>

namespace WCDB {

class BindParameter : public SQLSyntax<Syntax::BindParameter> {
public:
    using SQLSyntax<Syntax::BindParameter>::SQLSyntax;
    BindParameter(int n);
    explicit BindParameter(const SyntaxString& name);

    static BindParameters bindParameters(size_t count);

    static BindParameter at(const SyntaxString& name);
    static BindParameter colon(const SyntaxString& name);
    static BindParameter dollar(const SyntaxString& name);

private:
    static BindParameter
    bindParameter(const SyntaxString& name, const SyntaxType::Switch& switcher);
};

} // namespace WCDB

#endif /* BindParameter_hpp */
