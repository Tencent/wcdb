// Created by chenqiuwen on 2023/11/4.
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

package com.tencent.wcdb.fts;

import java.util.Map;

public class BuiltinTokenizer {
    /**
     * The following four are sqlite built-in fts tokenizers.
     * `Simple` tokenizer can be used in fts3/4 and the others can be used in fts3/4/5.
     * You can use `com.tencent.wcdb.FTSModule` annotation to config fts tokenizer for a Java ORM class.
     */
    public final static String Simple = "simple";
    public final static String Porter = "porter";
    public final static String ICU = "icu";
    public final static String Unicode61 = "unicode61";

    /**
     * `OneOrBinary` is a WCDB implemented tokenizer for fts3/4.
     * Consecutive English letters will be recognized as an English token, and this token will be stemmed using the porter stemming algorithm.
     * Continuous Arabic numerals will be recognized as a single numeric token.
     * For other Unicode characters, each character will be recognized as one token.
     * For example, the sentence "The phone number of 张三 is 12345" will be split into these tokens: "the", "phone", "number", "of", "张", "三", "is", "12345".
     * You can use `com.tencent.wcdb.FTSModule` annotation to config fts tokenizer for a Java ORM class.
     */
    public final static String OneOrBinary = "wcdb_one_or_binary";

    /**
     * Deprecated tokenizer for fts3/4.
     */
    public final static String MMICU = "mmicu";

    /**
     * `Verbatim` is a WCDB implemented tokenizer for fts5.
     * It has the same tokenize rules as `OneOrBinary`.
     * You can use `com.tencent.wcdb.FTSModule annotation` to config fts tokenizer for a Java ORM class.
     *
     * @see #OneOrBinary
     */
    public final static String Verbatim = "wcdb_verbatim";

    /**
     * `Pinyin` is a WCDB implemented tokenizer for fts5.
     * It is designed for pinyin search.
     * You can use the simplified or full pinyin of Chinese characters to search for Chinese characters.
     * Before using this tokenizer, you need to use `Database.configPinyinDict()` to configure the mapping relationship between Chinese characters and their pinyin.
     * You can use `com.tencent.wcdb.FTSModule annotation` to config fts tokenizer for a Java ORM class.
     *
     * @see com.tencent.wcdb.core.Database#configPinyinDict(Map)
     */
    public final static String Pinyin = "wcdb_pinyin";

    /**
     * The following two are optional parameters for WCDB implemented tokenizers.
     * You can configure these parameters on the `tokenizerParameters` attribute of `com.tencent.wcdb.FTSModule` annotation.
     */
    public static class Parameter {
        /**
         * `SimplifyChinese` enables the tokenizer to convert each traditional Chinese character into a simplified Chinese character,
         * so that you can use Simplified Chinese characters to search Traditional Chinese characters.
         * Note that you need to use `Database.configTraditionalChineseDict()` to config the mapping relationship between traditional Chinese characters and simplified Chinese characters.
         *
         * @see com.tencent.wcdb.core.Database#configTraditionalChineseDict(Map)
         */
        public final static String SimplifyChinese = "chinese_traditional_to_simplified";

        /**
         * `SkipStemming` will disable the stemming during tokenization.
         */
        public final static String SkipStemming = "skip_stemming";
    }
}
