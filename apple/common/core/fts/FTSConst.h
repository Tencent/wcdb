//
// Created by 陈秋文 on 2022/10/28.
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

namespace WCDB {

namespace BuiltinTokenizer {

static constexpr const char* Simple = "simple";
static constexpr const char* Porter = "porter";
static constexpr const char* ICU = "icu";
static constexpr const char* Unicode61 = "unicode61";

// WCDB implemented tokenizer
// Only for fts3/fts4
static constexpr const char* OneOrBinary = "wcdb_one_or_binary";
static constexpr const char* LegacyOneOrBinary
= "WCDB"; // Same as OneOrBinary, just for compatibility with older versions
// Only for fts5
static constexpr const char* Verbatim = "wcdb_verbatim";
static constexpr const char* Pinyin = "wcdb_pinyin";

// Optional parameters for WCDB implemented tokenizers
namespace Parameter {

static constexpr const char* NeedSymbol = "need_symbol";
static constexpr const char* SimplifyChinese
= "chinese_traditional_to_simplified";
static constexpr const char* SkipStemming = "skip_stemming";

} //namespace Parameter

} //namespace BuiltinTokenizer

namespace Module {

static constexpr const char* FTS3 = "fts3";
static constexpr const char* FTS4 = "fts4";
static constexpr const char* FTS5 = "fts5";

}; // namespace Module

namespace BuiltinAuxiliaryFunction {

static constexpr const char* SubstringMatchInfo = "substring_match_info";

} //namespace BuiltinAuxiliaryFunction

} //namespace WCDB
