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

RaiseFunction::RaiseFunction() = default;

RaiseFunction::~RaiseFunction() = default;

RaiseFunction& RaiseFunction::ignore()
{
    syntax().switcher = SyntaxType::Switch::Ignore;
    return *this;
}

RaiseFunction& RaiseFunction::rollback(const UnsafeStringView& errorMessage)
{
    syntax().switcher = SyntaxType::Switch::Rollback;
    syntax().errorMessage = errorMessage;
    return *this;
}

RaiseFunction& RaiseFunction::abort(const UnsafeStringView& errorMessage)
{
    syntax().switcher = SyntaxType::Switch::Abort;
    syntax().errorMessage = errorMessage;
    return *this;
}

RaiseFunction& RaiseFunction::fail(const UnsafeStringView& errorMessage)
{
    syntax().switcher = SyntaxType::Switch::Fail;
    syntax().errorMessage = errorMessage;
    return *this;
}

} // namespace WCDB
