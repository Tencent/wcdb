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

#ifndef Reporter_hpp
#define Reporter_hpp

#include <WCDB/Error.hpp>

namespace WCDB {

class Reporter {
public:
    typedef std::function<void(const Error &)> Callback;
    static Reporter *shared();

    void report(const Error &error);
    void setCallback(const Callback &callback);

    static void defaultCallback(const Error &error);
    static void logger(const Error::Level &level,
                       const std::string &description);

    void error(const std::string &message);
    void warning(const std::string &message);
    void fatal(const std::string &message);

protected:
    void report(Error::Level level, const std::string &message);

    Reporter();
    Callback m_callback;
};

} //namespace WCDB

#endif /* Reporter_hpp */
