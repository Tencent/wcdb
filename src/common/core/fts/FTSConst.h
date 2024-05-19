//
// Created by qiuwenchen on 2022/10/28.
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

namespace Module {
/**
 The following three are different versions of the sqlite fts module. `Module::FTS5` is recommended. You can use `WCDB_CPP_VIRTUAL_TABLE_MODULE` to config fts module for a cpp ORM class.
 */
static constexpr const char* FTS3 = "fts3";
static constexpr const char* FTS4 = "fts4";
static constexpr const char* FTS5 = "fts5";

}; // namespace Module

namespace BuiltinTokenizer {
/**
 The following four are sqlite built-in fts tokenizers. `WCDB::Simple` tokenizer can be used in fts3/4 and the others can be used in fts3/4/5. You can use `WCDB_CPP_VIRTUAL_TABLE_TOKENIZE` to config fts tokenizer for a cpp ORM class.
 */
static constexpr const char* Simple = "simple";
static constexpr const char* Porter = "porter";
static constexpr const char* ICU = "icu";
static constexpr const char* Unicode61 = "unicode61";

/**
 The following two are WCDB implemented tokenizers for fts3/4. They are the same tokenizer with different name.
 When tokenizing, symbol characters are ignored by default (Before using this tokenizer, you need to use `static Database::configSymbolDetector()` to configure a symbol detector.). Consecutive English letters will be recognized as an English token, and this token will be stemmed using the porter stemming algorithm. Continuous Arabic numerals will be recognized as a single numeric token. For other Unicode characters, each character will be recognized as one token.
 For example, the sentence "The phone number of 张三 is 12345" will be split into these tokens: "the", "phone", "number", "of", "张", "三", "is", "12345".
 */
static constexpr const char* OneOrBinary = "wcdb_one_or_binary";
static constexpr const char* LegacyOneOrBinary
= "WCDB"; // Same as OneOrBinary, just for compatibility with older versions

#ifdef __ANDROID__
static constexpr const char* MMICU = "mmicu";
#endif

/**
 The following two are WCDB implemented tokenizers for fts5.
 `BuiltinTokenizer::Verbatim` has the same tokenize rules as `BuiltinTokenizer::OneOrBinary` or `BuiltinTokenizer::LegacyOneOrBinary`.
 `BuiltinTokenizer::Pinyin` is designed for pinyin search. You can use the simplified or full pinyin of Chinese characters to search for Chinese characters. Before using this tokenizer, you need to use `static Database::configPinyinConverter()` to configure the converter from Chinese characters to their pinyins.
 */
static constexpr const char* Verbatim = "wcdb_verbatim";
static constexpr const char* Pinyin = "wcdb_pinyin";

namespace Parameter {
/**
 The following three are optional parameters for WCDB implemented tokenizers. You can use `WCDB_CPP_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS` to config fts tokenizer with parameters for a cpp ORM class.
 Configuring `BuiltinTokenizer::Parameter::NeedSymbol` allows the tokenizer to recognize each symbol character as a token.
 Configuring `BuiltinTokenizer::Parameter::SimplifyChinese` enables the tokenizer to convert each traditional Chinese character into a simplified Chinese character, so that you can use Simplified Chinese characters to search Traditional Chinese characters. Note that you need to use `static Database::configTraditionalChineseConverter()` to configure the converter from traditional Chinese characters to their simplified Chinese characters before using the tokenizer.
 Configuring `BuiltinTokenizer::Parameter::SkipStemming` will disable the stemming during tokenization.
 */
static constexpr const char* NeedSymbol = "need_symbol";
static constexpr const char* SimplifyChinese = "chinese_traditional_to_simplified";
static constexpr const char* SkipStemming = "skip_stemming";

} //namespace Parameter

} //namespace BuiltinTokenizer

namespace BuiltinAuxiliaryFunction {

/**
 `BuiltinAuxiliaryFunction::SubstringMatchInfo` is a WCDB implemented auxiliary function for fts5.
 When you need to concat multiple contents together with multi-level separators and save them in a single column of the fts5 table, 
 you can use this auxiliary function to improve the accuracy of searching for the contents of this column.
 Suppose you have a friend named 张三, whose address is 广州 and 上海, and phone number is 12345 and 67890. 
 You can use semicolon and comma as two-level separators to concat your friend's information into "张三;广州,上海;12345,67890" and save it in a column named "friends" in an fts5 table.
 Then you can use following code to search your friend:
 
        fts5Table.selectOneColumn(WCDB::Column(fts5Table.tableName).substringMatchInfo(0, ";,"), WCDB::Column("friends").match("州"));
 
 The first argument of SubstringMatchInfo is a column named with table name. 
 The second argument is the index of the colum you need to search. The index is are numbered starting from 0. 
 The third parameter is the separator used when concating the content, which should be arranged according to their level.
 Then you will get this string result "1,0;广;州;3;". 
 The result is made by concating a variety of information with the separator you passed in.
 The first part of the result "1,0" gives the hierarchical position of the matched content within matched column.
 The second part "广;州" is the original text of the matching content, you can use it for highlighting.
 The third part "3" is the byte offset of the matching content in the original text, you can use it to sort the results.
 
 @warning The search results may contain some empty strings, which are invalid results. 
 This kind of results appear when the content of some rows contain the tokens you are searching for,
 but these tokens are located in different parts separated by separators.
 You just need to ignore these results.
 */
static constexpr const char* SubstringMatchInfo = "substring_match_info";

} //namespace BuiltinAuxiliaryFunction

} //namespace WCDB
