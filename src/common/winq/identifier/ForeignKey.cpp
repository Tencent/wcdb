//
// Created by sanhuazhang on 2019/05/02
//

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

#include "WINQ.h"

namespace WCDB {

ForeignKey::ForeignKey() = default;

ForeignKey::~ForeignKey() = default;

ForeignKey& ForeignKey::references(const UnsafeStringView& table)
{
    syntax().foreignTable = table;
    return *this;
}

ForeignKey& ForeignKey::column(const Column& column)
{
    syntax().columns.push_back(column);
    return *this;
}

ForeignKey& ForeignKey::onDeleteSetNull()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnDeleteSetNull);
    return *this;
}

ForeignKey& ForeignKey::onDeleteSetDefault()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnDeleteSetDefault);
    return *this;
}

ForeignKey& ForeignKey::onDeleteCascade()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnDeleteCascade);
    return *this;
}

ForeignKey& ForeignKey::onDeleteRestrict()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnDeleteRestrict);
    return *this;
}

ForeignKey& ForeignKey::onDeleteNoAction()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnDeleteNoAction);
    return *this;
}

ForeignKey& ForeignKey::onUpdateSetNull()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnUpdateSetNull);
    return *this;
}

ForeignKey& ForeignKey::onUpdateSetDefault()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnUpdateSetDefault);
    return *this;
}

ForeignKey& ForeignKey::onUpdateCascade()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnUpdateCascade);
    return *this;
}

ForeignKey& ForeignKey::onUpdateRestrict()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnUpdateRestrict);
    return *this;
}

ForeignKey& ForeignKey::onUpdateNoAction()
{
    syntax().switchers.push_back(SyntaxType::Switch::OnUpdateNoAction);
    return *this;
}

ForeignKey& ForeignKey::match(const MatchType& type)
{
    syntax().matchType = type;
    return *this;
}

ForeignKey& ForeignKey::deferrable()
{
    syntax().deferrable = SyntaxType::Deferrable::Deferrable;
    return *this;
}

ForeignKey& ForeignKey::deferrableInitiallyDeferred()
{
    syntax().deferrable = SyntaxType::Deferrable::DeferrableInitiallyDeferred;
    return *this;
}

ForeignKey& ForeignKey::deferrableInitiallyImmediate()
{
    syntax().deferrable = SyntaxType::Deferrable::DeferrableInitiallyImmediate;
    return *this;
}

ForeignKey& ForeignKey::notDeferrable()
{
    syntax().deferrable = SyntaxType::Deferrable::NotDeferrable;
    return *this;
}

ForeignKey& ForeignKey::notDeferrableInitiallyDeferred()
{
    syntax().deferrable = SyntaxType::Deferrable::NotDeferrableInitiallyDeferred;
    return *this;
}

ForeignKey& ForeignKey::notDeferrableInitiallyImmediate()
{
    syntax().deferrable = SyntaxType::Deferrable::NotDeferrableInitiallyImmediate;
    return *this;
}

} // namespace WCDB
