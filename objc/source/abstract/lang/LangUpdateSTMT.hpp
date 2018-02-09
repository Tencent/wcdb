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

#ifndef LangUpdateSTMT_hpp
#define LangUpdateSTMT_hpp

#include <WINQ/lang_common.h>

namespace WCDB {

namespace lang {

class UpdateSTMT : public CRUDLang {
public:
    UpdateSTMT();

    copy_on_write_lazy_lang<WithClause> withClause;
    enum class Type : int {
        NotSet,
        Update,
        UpdateOrRollback,
        UpdateOrAbort,
        UpdateOrReplace,
        UpdateOrFail,
        UpdateOrIgnore,
    };
    Type type;

    copy_on_write_lazy_lang<QualifiedTableName> qualifiedTableName;

    class KeyValue : public Lang {
    public:
        copy_on_write_lazy_string_list keys;
        copy_on_write_lazy_lang<Expr> value;

        virtual copy_on_write_string SQL() const override;
    };
    copy_on_write_lazy_lang_list<KeyValue> keyValues;

    copy_on_write_lazy_lang<Expr> condition;
    copy_on_write_lazy_lang_list<OrderingTerm> orderingTerm;
    copy_on_write_lazy_lang<Expr> limit;
    bool offset;
    copy_on_write_lazy_lang<Expr> limitParameter;

    virtual copy_on_write_string SQL() const override;

protected:
    constexpr static const char *TypeName(const Type &type);
};

} // namespace lang

} // namespace WCDB

#endif /* LangUpdateSTMT_hpp */
