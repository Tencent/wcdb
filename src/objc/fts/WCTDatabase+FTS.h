//
// Created by sanhuazhang on 2019/05/02
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

#import "FTS5AuxiliaryFunctionTemplate.hpp"
#import "OneOrBinaryTokenizer.hpp"
#import "PinyinTokenizer.hpp"
#import "SubstringMatchInfo.hpp"
#import "WCTCommon.h"
#import "WCTDatabase.h"
#import "WCTFTSTokenizerUtil.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The following three are different versions of the sqlite fts module. `WCTModuleFTS5` is recommended. You can use `WCDB_VIRTUAL_TABLE_MODULE` to config fts module for a `WCTTableCoding` class.
 */
WCDB_EXTERN NSString* const WCTModuleFTS3;
WCDB_EXTERN NSString* const WCTModuleFTS4;
WCDB_EXTERN NSString* const WCTModuleFTS5;

/**
 The following four are sqlite built-in fts tokenizers. `WCTTokenizerSimple` can be used in fts3/4 and the others can be used in fts3/4/5. You can use `WCDB_VIRTUAL_TABLE_TOKENIZE` to config fts tokenizer for a `WCTTableCoding` class.
 */
WCDB_EXTERN NSString* const WCTTokenizerSimple;
WCDB_EXTERN NSString* const WCTTokenizerPorter;
WCDB_EXTERN NSString* const WCTTokenizerICU;
WCDB_EXTERN NSString* const WCTTokenizerUnicode61;

/**
 The following two are WCDB implemented tokenizers for fts3/4. They are the same tokenizer with different name.
 When tokenizing, symbol characters are ignored by default. Consecutive English letters will be recognized as an English token, and this token will be stemmed using the porter stemming algorithm. Continuous Arabic numerals will be recognized as a single numeric token. For other Unicode characters, each character will be recognized as one token.
 For example, the sentence "The phone number of 张三 is 12345" will be split into these tokens: "the", "phone", "number", "of", "张", "三", "is", "12345".
 */
WCDB_EXTERN NSString* const WCTTokenizerOneOrBinary;
WCDB_EXTERN NSString* const WCTTokenizerLegacyOneOrBinary; // compatible for WCTTokenizerNameWCDB before v1.1.0

/**
 The following two are WCDB implemented tokenizers for fts5.
 `WCTTokenizerVerbatim` has the same tokenize rules as `WCTTokenizerOneOrBinary` or `WCTTokenizerLegacyOneOrBinary`.
 `WCTTokenizerPinyin` is designed for pinyin search. You can use the simplified or full pinyin of Chinese characters to search for Chinese characters. Before using this tokenizer, you need to use `+[WCTDatabase configPinYinDict:]` to configure the mapping relationship between Chinese characters and their pinyin.
 */
WCDB_EXTERN NSString* const WCTTokenizerVerbatim;
WCDB_EXTERN NSString* const WCTTokenizerPinyin;

/**
 The following three are optional parameters for WCDB implemented tokenizers. You can use `WCDB_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS` to config fts tokenizer with parameters for a `WCTTableCoding` class.
 Configuring `WCTTokenizerParameter_NeedSymbol` allows the tokenizer to recognize each symbol character as a token.
 Configuring `WCTTokenizerParameter_SimplifyChinese` enables the tokenizer to convert each traditional Chinese character into a simplified Chinese character, so that you can use Simplified Chinese characters to search Traditional Chinese characters. Note that you need to use `+[WCTDatabase configTraditionalChineseDict:]` to configure the mapping relationship between traditional Chinese characters and simplified Chinese characters before using the tokenizer.
 Configuring `WCTTokenizerParameter_SkipStemming` will disable the stemming during tokenization.
 */
WCDB_EXTERN NSString* const WCTTokenizerParameter_NeedSymbol;
WCDB_EXTERN NSString* const WCTTokenizerParameter_SimplifyChinese;
WCDB_EXTERN NSString* const WCTTokenizerParameter_SkipStemming;

/**
 `WCTAuxiliaryFunction_SubstringMatchInfo` is a WCDB implemented auxiliary function for fts5.
 When you need to concat multiple contents together with multi-level separators and save them in a single column of the fts5 table, you can use this auxiliary function to improve the accuracy of searching for the contents of this column.
 Suppose you have a friend named 张三, whose address is 广州 and 上海, and phone number is 12345 and 67890. You can use semicolon and comma as two-level separators to concat your friend's information into "张三;广州,上海;12345,67890" and save it in a column named "friends" in an fts5 table. Then you can use following code to search your friend:
 
        [fts5Table getColumnOnResultColumn:WCDB::Column(fts5Table.tableName).substringMatchInfo(0, ";,") where:WCDB::Column("friends").match("州")];
 
 The first argument of SubstringMatchInfo is a column named with table name. The second argument is the index of the colum you need to search. The index is are numbered starting from 0. The third parameter is the separator used when concating the content, which should be arranged according to their level.
 Then you will get this string result "1,0;广;州;3;". The result is made by concating a variety of information with the separator you passed in. The first part of the result "1,0" gives the hierarchical position of the matched content within matched column. The second part "广;州" is the original text of the matching content, you can use it for highlighting. The third part "3" is the byte offset of the matching content in the original text, you can use it to sort the results.
 
 @warning The search results may contain some empty strings, which are invalid results. This kind of results appear when the content of some rows contain the tokens you are searching for, but these tokens are located in different parts separated by separators. You just need to ignore these results.
 */
WCDB_EXTERN NSString* const WCTAuxiliaryFunction_SubstringMatchInfo;

WCDB_API @interface WCTDatabase(FTS)

/**
 @brief Enable to automatically merge b-tree segments of fts5 index after modifying the content in fts5 table.
 The merge operation will be performed in a separate sub-thread, and will be suspended when it detects that other threads need to modify the fts table. So it will improve the performance of modifying the fts5 table.
 Using auto-merge, WCDB can maintain only one segment per level in the fts5 index structure. So it will significantly improve the search performance of the fts5 index.
 @param flag to enable auto-merge.
 */
- (void)enableAutoMergeFTS5Index:(BOOL)flag;

/**
 @brief Setup tokenizer with name for current database.
 @Note  You can set up the built-in tokenizers of sqlite and the tokenizers implemented by WCDB directly. If you want to use your custom tokenizer, you should firstly register it through `+[WCTDatabase registerTokenizer:named:]`.
 @see   `+[WCTDatabase registerTokenizer:named:]`
 */
- (void)addTokenizer:(NSString*)tokenizerName;

/**
 @brief Register custom tokenizer.
 @Note  The custom tokenizer needs to inherit `WCDB::AbstractFTSTokenizer`.
 @param module Tokenizer module for fts. You can use WCDB::FTS3TokenizerModuleTemplate to construct your custom tokenizer as tokenizer module for fts3/4, or use WCDB::FTS5TokenizerModuleTemplate to construct for fts5.
 @param name Name of tokenizer.
 */
+ (void)registerTokenizer:(const WCDB::TokenizerModule&)module named:(NSString*)name;

/**
 @brief Setup auxiliary function with name for current database.
 @Note  You can set up the built-in auxiliary functions of sqlite and the auxiliary functions implemented by WCDB directly. If you want to use your custom auxiliary function, you should firstly register it through `+[WCTDatabase registerAuxiliaryFunction:named:]`.
 @see   `+[WCTDatabase registerAuxiliaryFunction:named:]`
 */
- (void)addAuxiliaryFunction:(NSString*)auxiliaryFunctionName;

/**
 @brief Register custom auxiliary function for fts5.
 @Note  The custom auxiliary function needs to inherit `WCDB::AbstractFTS5AuxiliaryFunctionObject`.
 @param module Auxiliary function module for fts5. You can use WCDB::FTS5AuxiliaryFunctionTemplate to construct your custom auxiliary function as auxiliary function module.
 @param name Name of auxiliary function.
 */
+ (void)registerAuxiliaryFunction:(const WCDB::FTS5AuxiliaryFunctionModule&)module named:(NSString*)name;

/**
 @brief Configure the mapping relationship between Chinese characters and their pinyin.
 @see   `WCTTokenizerPinyin`
 @param pinyinDict Pinyin mapping dictionary.
 */
+ (void)configPinYinDict:(NSDictionary<NSString* /*Chinese character*/, NSArray<NSString*>*>* /*Pinyin array*/)pinyinDict;

/**
 @brief Configure the mapping relationship between traditional Chinese characters and simplified Chinese characters.
 @see   `WCTTokenizerParameter_SimplifyChinese`
 @param traditionalChineseDict Traditional Chinese mapping dictionary.
 */
+ (void)configTraditionalChineseDict:(NSDictionary<NSString* /*Traditional Chinese character*/, NSString* /*Simplified Chinese character*/>*)traditionalChineseDict;

@end

NS_ASSUME_NONNULL_END
