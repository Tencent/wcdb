//
// Created by qiuwenchen on 2022/3/7.
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

#include "FTSError.hpp"
#include "SQLite.h"

namespace WCDB {
int FTSError::OK()
{
    return SQLITE_OK;
}

int FTSError::Done()
{
    return SQLITE_DONE;
}

int FTSError::NoMem()
{
    return SQLITE_NOMEM;
}

int FTSError::Error()
{
    return SQLITE_ERROR;
}

bool FTSError::isOK(int rc)
{
    return rc == SQLITE_OK;
}

bool FTSError::isDone(int rc)
{
    return rc == SQLITE_DONE;
}

} // namespace WCDB
