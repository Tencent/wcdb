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

#include <WCDB/column.hpp>
#include <WCDB/column_def.hpp>
#include <WCDB/column_index.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/foreign_key.hpp>
#include <WCDB/module_argument.hpp>
#include <WCDB/table_constraint.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

std::string stringByReplacingOccurrencesOfString(const std::string &origin,
                                                 const std::string &target,
                                                 const std::string &replacement)
{
    bool replace = false;
    size_t last = 0;
    size_t found = 0;
    std::string output;
    while ((found = origin.find(target, last)) != std::string::npos) {
        if (!replace) {
            replace = true;
            output.clear();
        }
        std::string sub = origin.substr(last, found - last);
        output += sub;
        output += replacement;
        last = found + target.length();
    }
    if (replace) {
        output += origin.substr(last, -1);
    }
    return replace ? output : origin;
}

} //namespace WCDB
