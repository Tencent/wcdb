//
// Created by qiuwenchen on 2022/8/1.
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

#include <WCDB/Core.hpp>
#include <WCDB/FileManager.hpp>
#include <stdio.h>
namespace WCDB {

void Core::print(const UnsafeStringView &message)
{
    printf("%s", message.data());
}

StringView FileManager::getTemporaryDirectory()
{
    return StringView();
}

bool FileManager::setFileProtection(const WCDB::UnsafeStringView &path,
                                    WCDB::FileProtection fileProtection)
{
    WCDB_UNUSED(path)
    WCDB_UNUSED(fileProtection)
    return true;
}

Optional<WCDB::FileProtection>
FileManager::getFileProtection(const WCDB::UnsafeStringView &path)
{
    WCDB_UNUSED(path)
    return WCDB::FileProtection::None;
}

} // namespace WCDB
