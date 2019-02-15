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

#ifndef __WCDB_TOKENIZER_CONFIG_HPP
#define __WCDB_TOKENIZER_CONFIG_HPP

#include <WCDB/Config.hpp>
#include <WCDB/TokenizerModule.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class Modules;

class TokenizerConfig final : public Config {
public:
    TokenizerConfig(const std::map<String, TokenizerModule>& modules);
    bool invoke(Handle* handle) override final;
#warning TODO - uninvoke and bool reinvokable

protected:
    const std::map<String, TokenizerModule> m_modules;
    const StatementSelect m_fts3Tokenizer;
};

} //namespace WCDB

#endif /* __WCDB_TOKENIZER_CONFIG_HPP */
