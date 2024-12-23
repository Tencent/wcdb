// Created by chenqiuwen on 2024/1/1.
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

public class BuiltinFTSAuxiliaryFunction {
    /**
     * `SubstringMatchInfo` is a WCDB implemented auxiliary function for fts5.
     * When you need to concat multiple contents together with multi-level separators and save them in a single column of the fts5 table,
     * you can use this auxiliary function to improve the accuracy of searching for the contents of this column.
     * Suppose you have a friend named 张三, whose address is 广州 and 上海, and phone number is 12345 and 67890.
     * You can use semicolon and comma as two-level separators to concat your friend's information into "张三;广州,上海;12345,67890" and save it in a column named "friends" in an fts5 table.
     * Then you can use following code to search your friend:
     *
     * <pre>
     *      <code>
     *          fts5Table.getOneColumnString(new Column(fts5Table.tableName).substringMatchInfo(0, ";,"), new Column("friends").match("州"));
     *      </code>
     *  </pre>
     * <p>
     * The first argument of SubstringMatchInfo is a column named with table name.
     * The second argument is the index of the colum you need to search. The index is are numbered starting from 0.
     * The third parameter is the separator used when concating the content, which should be arranged according to their level.
     * Then you will get this string result "1,0;广;州;3;".
     * The result is made by concating a variety of information with the separator you passed in.
     * The first part of the result "1,0" gives the hierarchical position of the matched content within matched column.
     * The second part "广;州" is the original text of the matching content, you can use it for highlighting.
     * The third part "3" is the byte offset of the matching content in the original text, you can use it to sort the results.
     * Note that The search results may contain some empty strings, which are invalid results.
     * This kind of results appear when the content of some rows contain the tokens you are searching for,
     * but these tokens are located in different parts separated by separators.
     * You just need to ignore these results.
     */
    public final static String SubstringMatchInfo = "substring_match_info";
}
