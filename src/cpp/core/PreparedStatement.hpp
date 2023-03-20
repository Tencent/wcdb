//
// Created by qiuwenchen on 2022/8/3.
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

#pragma once
#include "CPPDeclaration.h"
#include "Statement.hpp"
#include "StatementOperation.hpp"

namespace WCDB {

class PreparedStatement final : public StatementOperation {
    friend class Handle;

public:
    PreparedStatement(PreparedStatement &&other);
    ~PreparedStatement() override final;

protected:
    PreparedStatement() = delete;
    PreparedStatement(const PreparedStatement &) = delete;
    PreparedStatement &operator=(const PreparedStatement &) = delete;
    PreparedStatement(HandleStatement *handleStatement);

    HandleStatement *getInnerHandleStatement() override final;
    using StatementOperation::prepare;
    using StatementOperation::isPrepared;
    using StatementOperation::finalize;

private:
    HandleStatement *m_innerHandleStatement;
};

typedef Optional<PreparedStatement> OptionalPreparedStatement;

} //namespace WCDB
