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

public class BuiltinTokenizer {
    // SQLite builtin tokenizer
    public final static String Simple = "simple";
    public final static String Porter = "porter";
    public final static String ICU = "icu";
    public final static String Unicode61 = "unicode61";

    // WCDB Builtin tokenizer
    public final static String OneOrBinary = "wcdb_one_or_binary";// Only for FTS3/4
    public final static String MMICU = "mmicu";
    public final static String Verbatim = "wcdb_verbatim";// Only for FTS5
    public final static String Pinyin = "wcdb_pinyin";// Only for FTS5

    // Tokenizer parameters for WCDB Builtin tokenizer
    public class Parameter {
        public final static String NeedSymbol = "need_symbol";
        public final static String SimplifyChinese = "chinese_traditional_to_simplified";
        public final static String SkipStemming = "skip_stemming";
    }
}
