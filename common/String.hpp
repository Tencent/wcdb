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

#ifndef String_hpp
#define String_hpp

#include <string>

namespace WCDB {

namespace String {

const std::string &empty();

struct CaseInsensiveComparator {
    bool operator()(const std::string &lhs, const std::string &rhs) const;
};
typedef struct CaseInsensiveComparator CaseInsensiveComparator;

bool isCaseInsensiveEqual(const std::string &lhs, const std::string &rhs);
bool hasCaseInsensivePrefix(const std::string &origin, const std::string &target);
bool hasPrefix(const std::string &origin, const std::string &target);

std::string replacingOccurrencesOfString(const std::string &origin,
                                         const std::string &target,
                                         const std::string &replacement);

std::string formatted(const char *format, ...);

uint32_t hash(const std::string &source);

} //namespace String

} //namespace WCDB

#endif /* Utility_hpp */
