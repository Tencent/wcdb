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

#ifndef CopyOnWriteString_hpp
#define CopyOnWriteString_hpp

#include <WCDB/CopyOnWrite.hpp>
#include <string>

class CopyOnWriteString : public CopyOnWrite<std::string> {
public:
    using Super = CopyOnWrite<std::string>;

    CopyOnWriteString();

    CopyOnWriteString(const std::string &string);

    CopyOnWriteString(const char *string);

    //    void append(const std::string &string);
    //
    //    void append(const CopyOnWriteString &string);

    bool empty() const;
};

#endif /* CopyOnWriteString_hpp */
