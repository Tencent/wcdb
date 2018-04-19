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

#include <WCDB/Core.h>

namespace WCDB {

CoreError::CoreError(const HandleError &handleError) : HandleError(handleError)
{
}

CoreError::CoreError(const Tag &tag_, const std::string &message_)
    : HandleError()
{
    code = (int) Error::Code::Error;
    tag = tag_;
    message = message_;
}

CoreError &CoreError::operator=(const HandleError &handleError)
{
    HandleError::operator=(handleError);
    return *this;
}

int CoreError::getType() const
{
    return CoreError::type;
}

} //namespace WCDB
