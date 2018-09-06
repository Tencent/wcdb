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

#ifndef TokenizeConfig_hpp
#define TokenizeConfig_hpp

#include <WCDB/Config.hpp>

namespace WCDB {

namespace FTS {
class Modules;
}

class TokenizeConfig : public Config {
public:
    TokenizeConfig(const std::list<std::string>& tokenizeNames, FTS::Modules* modules);
    bool invoke(Handle* handle) override;

protected:
    const FTS::Modules* m_modules;
    const StatementSelect m_fts3Tokenizer;
    const std::list<std::string> m_tokenizeNames;
};

} //namespace WCDB

#endif /* TokenizeConfig_hpp */
