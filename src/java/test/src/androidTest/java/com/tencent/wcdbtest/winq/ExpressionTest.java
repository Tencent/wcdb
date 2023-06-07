// Created by chenqiuwen on 2023/4/2.
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

package com.tencent.wcdbtest.winq;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import com.tencent.wcdb.winq.*;
import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.List;
import java.util.HashSet;
import java.util.ArrayList;
import java.util.Set;

@RunWith(AndroidJUnit4.class)
public class ExpressionTest {
    @Test
    public void testExpression() {
        winqEqual(new Expression(new LiteralValue(1)), "1");
        winqEqual(new Expression(new LiteralValue(1.1)), "1.1000000000000001");
        winqEqual(new Expression(new LiteralValue("abc")), "'abc'");
        winqEqual(new Expression(new LiteralValue(false)), "FALSE");
        winqEqual(new Expression(new LiteralValue(true)), "TRUE");
        winqEqual(new Expression(new Column("testColumn")), "testColumn");
        winqEqual(new Expression(new BindParameter(1)), "?1");
    }

    @Test
    public void testUnaryOperation() {
        Column column = new Column("testColumn");
        winqEqual(column.isNull(), "testColumn ISNULL");
        winqEqual(column.notNull(), "testColumn NOTNULL");
    }

    @Test
    public void testBinaryOperation() {
        Column left = new Column("left");
        Column right = new Column("right");

        winqEqual(left.or(right), "left OR right");
        winqEqual(left.and(right), "left AND right");

        winqEqual(left.multiply(right), "left * right");
        winqEqual(left.multiply(1), "left * 1");
        winqEqual(left.multiply(1.1), "left * 1.1000000000000001");

        winqEqual(left.divide(right), "left / right");
        winqEqual(left.divide(1), "left / 1");
        winqEqual(left.divide(1.1), "left / 1.1000000000000001");

        winqEqual(left.mod(right), "left % right");
        winqEqual(left.mod(1), "left % 1");
        winqEqual(left.mod(1.1), "left % 1.1000000000000001");

        winqEqual(left.add(right), "left + right");
        winqEqual(left.add(1), "left + 1");
        winqEqual(left.add(1.1), "left + 1.1000000000000001");

        winqEqual(left.minus(right), "left - right");
        winqEqual(left.minus(1), "left - 1");
        winqEqual(left.minus(1.1), "left - 1.1000000000000001");

        winqEqual(left.leftShift(right), "left << right");
        winqEqual(left.leftShift(1), "left << 1");

        winqEqual(left.rightShift(right), "left >> right");
        winqEqual(left.rightShift(1), "left >> 1");

        winqEqual(left.bitAnd(right), "left & right");
        winqEqual(left.bitAnd(1), "left & 1");

        winqEqual(left.bitOr(right), "left | right");
        winqEqual(left.bitOr(1), "left | 1");

        winqEqual(left.lt(right), "left < right");
        winqEqual(left.lt(1), "left < 1");
        winqEqual(left.lt(1.1), "left < 1.1000000000000001");
        winqEqual(left.lt("abc"), "left < 'abc'");

        winqEqual(left.le(right), "left <= right");
        winqEqual(left.le(1), "left <= 1");
        winqEqual(left.le(1.1), "left <= 1.1000000000000001");
        winqEqual(left.le("abc"), "left <= 'abc'");

        winqEqual(left.gt(right), "left > right");
        winqEqual(left.gt(1), "left > 1");
        winqEqual(left.gt(1.1), "left > 1.1000000000000001");
        winqEqual(left.gt("abc"), "left > 'abc'");

        winqEqual(left.ge(right), "left >= right");
        winqEqual(left.ge(1), "left >= 1");
        winqEqual(left.ge(1.1), "left >= 1.1000000000000001");
        winqEqual(left.ge("abc"), "left >= 'abc'");

        winqEqual(left.eq(right), "left == right");
        winqEqual(left.eq(false), "left == FALSE");
        winqEqual(left.eq(1), "left == 1");
        winqEqual(left.eq(1.1), "left == 1.1000000000000001");
        winqEqual(left.eq("abc"), "left == 'abc'");

        winqEqual(left.notEq(right), "left != right");
        winqEqual(left.notEq(false), "left != FALSE");
        winqEqual(left.notEq(1), "left != 1");
        winqEqual(left.notEq(1.1), "left != 1.1000000000000001");
        winqEqual(left.notEq("abc"), "left != 'abc'");

        winqEqual(left.concat(right), "left || right");
        winqEqual(left.concat(1), "left || 1");
        winqEqual(left.concat(1.1), "left || 1.1000000000000001");
        winqEqual(left.concat("abc"), "left || 'abc'");
    }

    @Test
    public void testBetweenOperation() {
        Column column = new Column("testColumn");
        Column start = new Column("start");
        Column end = new Column("end");


        winqEqual(column.between(start, end), "testColumn BETWEEN start AND end");
        winqEqual(column.between(start, 1), "testColumn BETWEEN start AND 1");
        winqEqual(column.between(start, 1.1), "testColumn BETWEEN start AND 1.1000000000000001");
        winqEqual(column.between(start, "abc"), "testColumn BETWEEN start AND 'abc'");

        winqEqual(column.between(1, end), "testColumn BETWEEN 1 AND end");
        winqEqual(column.between(1, 1), "testColumn BETWEEN 1 AND 1");
        winqEqual(column.between(1, 1.1), "testColumn BETWEEN 1 AND 1.1000000000000001");
        winqEqual(column.between(1, "abc"), "testColumn BETWEEN 1 AND 'abc'");

        winqEqual(column.between(1.1, end), "testColumn BETWEEN 1.1000000000000001 AND end");
        winqEqual(column.between(1.1, 1), "testColumn BETWEEN 1.1000000000000001 AND 1");
        winqEqual(column.between(1.1, 1.1), "testColumn BETWEEN 1.1000000000000001 AND 1.1000000000000001");
        winqEqual(column.between(1.1, "abc"), "testColumn BETWEEN 1.1000000000000001 AND 'abc'");

        winqEqual(column.between("abc", end), "testColumn BETWEEN 'abc' AND end");
        winqEqual(column.between("abc", 1), "testColumn BETWEEN 'abc' AND 1");
        winqEqual(column.between("abc", 1.1), "testColumn BETWEEN 'abc' AND 1.1000000000000001");
        winqEqual(column.between("abc", "abc"), "testColumn BETWEEN 'abc' AND 'abc'");

        winqEqual(column.notBetween(start, end), "testColumn NOT BETWEEN start AND end");
        winqEqual(column.notBetween(start, 1), "testColumn NOT BETWEEN start AND 1");
        winqEqual(column.notBetween(start, 1.1), "testColumn NOT BETWEEN start AND 1.1000000000000001");
        winqEqual(column.notBetween(start, "abc"), "testColumn NOT BETWEEN start AND 'abc'");

        winqEqual(column.notBetween(1, end), "testColumn NOT BETWEEN 1 AND end");
        winqEqual(column.notBetween(1, 1), "testColumn NOT BETWEEN 1 AND 1");
        winqEqual(column.notBetween(1, 1.1), "testColumn NOT BETWEEN 1 AND 1.1000000000000001");
        winqEqual(column.notBetween(1, "abc"), "testColumn NOT BETWEEN 1 AND 'abc'");

        winqEqual(column.notBetween(1.1, end), "testColumn NOT BETWEEN 1.1000000000000001 AND end");
        winqEqual(column.notBetween(1.1, 1), "testColumn NOT BETWEEN 1.1000000000000001 AND 1");
        winqEqual(column.notBetween(1.1, 1.1), "testColumn NOT BETWEEN 1.1000000000000001 AND 1.1000000000000001");
        winqEqual(column.notBetween(1.1, "abc"), "testColumn NOT BETWEEN 1.1000000000000001 AND 'abc'");

        winqEqual(column.notBetween("abc", end), "testColumn NOT BETWEEN 'abc' AND end");
        winqEqual(column.notBetween("abc", 1), "testColumn NOT BETWEEN 'abc' AND 1");
        winqEqual(column.notBetween("abc", 1.1), "testColumn NOT BETWEEN 'abc' AND 1.1000000000000001");
        winqEqual(column.notBetween("abc", "abc"), "testColumn NOT BETWEEN 'abc' AND 'abc'");
    }

    @Test
    public void testInOperation() {
        Column column = new Column("testColumn");

        winqEqual(column.in(new char[]{'a', 'b', 'c'}), "testColumn IN(97, 98, 99)");
        Set<Character> charSet = new HashSet<Character>();
        charSet.add('a');
        charSet.add('b');
        charSet.add('c');
        winqEqual(column.in(charSet), "testColumn IN(97, 98, 99)");
        winqEqual(column.in(new Character[]{'a', 'b', 'c'}), "testColumn IN(97, 98, 99)");
        List<Character> charList = new ArrayList<Character>();
        charList.add('a');
        charList.add('b');
        charList.add('c');
        winqEqual(column.in(charList), "testColumn IN(97, 98, 99)");

        winqEqual(column.in(new short[]{1, 2, 3}), "testColumn IN(1, 2, 3)");
        Set<Short> shortSet = new HashSet<Short>();
        shortSet.add((short) 1);
        shortSet.add((short)2);
        shortSet.add((short)3);
        winqEqual(column.in(shortSet), "testColumn IN(1, 2, 3)");
        winqEqual(column.in(new Short[]{1, 2, 3}), "testColumn IN(1, 2, 3)");
        List<Short> shortList = new ArrayList<Short>();
        shortList.add((short) 1);
        shortList.add((short) 2);
        shortList.add((short) 3);
        winqEqual(column.in(shortList), "testColumn IN(1, 2, 3)");

        winqEqual(column.in(new int[]{1, 2, 3}), "testColumn IN(1, 2, 3)");
        Set<Integer> intSet = new HashSet<Integer>();
        intSet.add(1);
        intSet.add(2);
        intSet.add(3);
        winqEqual(column.in(intSet), "testColumn IN(1, 2, 3)");
        winqEqual(column.in(new Integer[]{1, 2, 3}), "testColumn IN(1, 2, 3)");
        List<Integer> intList = new ArrayList<Integer>();
        intList.add(1);
        intList.add(2);
        intList.add(3);
        winqEqual(column.in(intList), "testColumn IN(1, 2, 3)");

        winqEqual(column.in(new long[]{1, 2, 3}), "testColumn IN(1, 2, 3)");
        Set<Long> longSet = new HashSet<Long>();
        longSet.add(1L);
        longSet.add(2L);
        longSet.add(3L);
        winqEqual(column.in(longSet), "testColumn IN(1, 2, 3)");
        winqEqual(column.in(new Long[]{1L, 2L, 3L}), "testColumn IN(1, 2, 3)");
        List<Long> longList = new ArrayList<Long>();
        longList.add(1L);
        longList.add(2L);
        longList.add(3L);
        winqEqual(column.in(longList), "testColumn IN(1, 2, 3)");

        winqEqual(column.in(new float[]{1.1f, 2.1f, 3.1f}), "testColumn IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)");
        Set<Float> floatSet = new HashSet<Float>();
        floatSet.add(1.1f);
        floatSet.add(2.1f);
        floatSet.add(3.1f);
        winqEqual(column.in(floatSet), "testColumn IN(2.0999999046325684, 3.0999999046325684, 1.1000000238418579)");
        winqEqual(column.in(new Float[]{1.1f, 2.1f, 3.1f}), "testColumn IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)");
        List<Float> floatList = new ArrayList<Float>();
        floatList.add(1.1f);
        floatList.add(2.1f);
        floatList.add(3.1f);
        winqEqual(column.in(floatList), "testColumn IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)");

        winqEqual(column.in(new double[]{1.1, 2.1, 3.1}), "testColumn IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)");
        Set<Double> doubleSet = new HashSet<Double>();
        doubleSet.add(1.1);
        doubleSet.add(2.1);
        doubleSet.add(3.1);
        winqEqual(column.in(doubleSet), "testColumn IN(3.1000000000000001, 1.1000000000000001, 2.1000000000000001)");
        winqEqual(column.in(new Double[]{1.1, 2.1, 3.1}), "testColumn IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)");
        List<Double> doubleList = new ArrayList<Double>();
        doubleList.add(1.1);
        doubleList.add(2.1);
        doubleList.add(3.1);
        winqEqual(column.in(doubleList), "testColumn IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)");

        winqEqual(column.in(new String[]{"abc", "def", "ghi"}), "testColumn IN('abc', 'def', 'ghi')");
        Set<String> stringSet = new HashSet<String>();
        stringSet.add("abc");
        stringSet.add("def");
        stringSet.add("ghi");
        winqEqual(column.in(stringSet), "testColumn IN('abc', 'def', 'ghi')");
        List<String> stringList = new ArrayList<String>();
        stringList.add("abc");
        stringList.add("def");
        stringList.add("ghi");
        winqEqual(column.in(stringList), "testColumn IN('abc', 'def', 'ghi')");

        winqEqual(column.inTable("testTable"), "testColumn IN testTable");
        winqEqual(column.inTable("testTable").schema(Schema.temp()), "testColumn IN temp.testTable");
        winqEqual(column.inFunction("testFunction").schema(Schema.main()).argument(1).argument("abc"), "testColumn IN main.testFunction(1, 'abc')");
    }

    @Test
    public void testNotInOperation() {
        Column column = new Column("testColumn");

        winqEqual(column.notIn(new char[]{'a', 'b', 'c'}), "testColumn NOT IN(97, 98, 99)");
        Set<Character> charSet = new HashSet<Character>();
        charSet.add('a');
        charSet.add('b');
        charSet.add('c');
        winqEqual(column.notIn(charSet), "testColumn NOT IN(97, 98, 99)");
        winqEqual(column.notIn(new Character[]{'a', 'b', 'c'}), "testColumn NOT IN(97, 98, 99)");
        List<Character> charList = new ArrayList<Character>();
        charList.add('a');
        charList.add('b');
        charList.add('c');
        winqEqual(column.notIn(charList), "testColumn NOT IN(97, 98, 99)");

        winqEqual(column.notIn(new short[]{1, 2, 3}), "testColumn NOT IN(1, 2, 3)");
        Set<Short> shortSet = new HashSet<Short>();
        shortSet.add((short) 1);
        shortSet.add((short)2);
        shortSet.add((short)3);
        winqEqual(column.notIn(shortSet), "testColumn NOT IN(1, 2, 3)");
        winqEqual(column.notIn(new Short[]{1, 2, 3}), "testColumn NOT IN(1, 2, 3)");
        List<Short> shortList = new ArrayList<Short>();
        shortList.add((short) 1);
        shortList.add((short) 2);
        shortList.add((short) 3);
        winqEqual(column.notIn(shortList), "testColumn NOT IN(1, 2, 3)");

        winqEqual(column.notIn(new int[]{1, 2, 3}), "testColumn NOT IN(1, 2, 3)");
        Set<Integer> intSet = new HashSet<Integer>();
        intSet.add(1);
        intSet.add(2);
        intSet.add(3);
        winqEqual(column.notIn(intSet), "testColumn NOT IN(1, 2, 3)");
        winqEqual(column.notIn(new Integer[]{1, 2, 3}), "testColumn NOT IN(1, 2, 3)");
        List<Integer> intList = new ArrayList<Integer>();
        intList.add(1);
        intList.add(2);
        intList.add(3);
        winqEqual(column.notIn(intList), "testColumn NOT IN(1, 2, 3)");

        winqEqual(column.notIn(new long[]{1, 2, 3}), "testColumn NOT IN(1, 2, 3)");
        Set<Long> longSet = new HashSet<Long>();
        longSet.add(1L);
        longSet.add(2L);
        longSet.add(3L);
        winqEqual(column.notIn(longSet), "testColumn NOT IN(1, 2, 3)");
        winqEqual(column.notIn(new Long[]{1L, 2L, 3L}), "testColumn NOT IN(1, 2, 3)");
        List<Long> longList = new ArrayList<Long>();
        longList.add(1L);
        longList.add(2L);
        longList.add(3L);
        winqEqual(column.notIn(longList), "testColumn NOT IN(1, 2, 3)");

        winqEqual(column.notIn(new float[]{1.1f, 2.1f, 3.1f}), "testColumn NOT IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)");
        Set<Float> floatSet = new HashSet<Float>();
        floatSet.add(1.1f);
        floatSet.add(2.1f);
        floatSet.add(3.1f);
        winqEqual(column.notIn(floatSet), "testColumn NOT IN(2.0999999046325684, 3.0999999046325684, 1.1000000238418579)");
        winqEqual(column.notIn(new Float[]{1.1f, 2.1f, 3.1f}), "testColumn NOT IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)");
        List<Float> floatList = new ArrayList<Float>();
        floatList.add(1.1f);
        floatList.add(2.1f);
        floatList.add(3.1f);
        winqEqual(column.notIn(floatList), "testColumn NOT IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)");

        winqEqual(column.notIn(new double[]{1.1, 2.1, 3.1}), "testColumn NOT IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)");
        Set<Double> doubleSet = new HashSet<Double>();
        doubleSet.add(1.1);
        doubleSet.add(2.1);
        doubleSet.add(3.1);
        winqEqual(column.notIn(doubleSet), "testColumn NOT IN(3.1000000000000001, 1.1000000000000001, 2.1000000000000001)");
        winqEqual(column.notIn(new Double[]{1.1, 2.1, 3.1}), "testColumn NOT IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)");
        List<Double> doubleList = new ArrayList<Double>();
        doubleList.add(1.1);
        doubleList.add(2.1);
        doubleList.add(3.1);
        winqEqual(column.notIn(doubleList), "testColumn NOT IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)");

        winqEqual(column.notIn(new String[]{"abc", "def", "ghi"}), "testColumn NOT IN('abc', 'def', 'ghi')");
        Set<String> stringSet = new HashSet<String>();
        stringSet.add("abc");
        stringSet.add("def");
        stringSet.add("ghi");
        winqEqual(column.notIn(stringSet), "testColumn NOT IN('abc', 'def', 'ghi')");
        List<String> stringList = new ArrayList<String>();
        stringList.add("abc");
        stringList.add("def");
        stringList.add("ghi");
        winqEqual(column.notIn(stringList), "testColumn NOT IN('abc', 'def', 'ghi')");

        winqEqual(column.notInTable("testTable"), "testColumn NOT IN testTable");
        winqEqual(column.notInTable("testTable").schema(Schema.temp()), "testColumn NOT IN temp.testTable");
        winqEqual(column.notInFunction("testFunction").schema(Schema.main()).argument(1).argument("abc"), "testColumn NOT IN main.testFunction(1, 'abc')");
    }

    @Test
    public void testCollate() {
        winqEqual(new Column("testColumn").collate("BINARY"), "testColumn COLLATE BINARY");
    }

    @Test
    public void testFunction() {
        Column left = new Column("left");
        String right = "right";

        winqEqual(left.substr(1, 2), "SUBSTR(left, 1, 2)");

        winqEqual(left.like(right), "left LIKE 'right'");

        winqEqual(left.glob(right), "left GLOB 'right'");

        winqEqual(left.match(right), "left MATCH 'right'");

        winqEqual(left.regexp(right), "left REGEXP 'right'");

        winqEqual(left.notLike(right), "left NOT LIKE 'right'");

        winqEqual(left.notGlob(right), "left NOT GLOB 'right'");

        winqEqual(left.notMatch(right), "left NOT MATCH 'right'");

        winqEqual(left.notRegexp(right), "left NOT REGEXP 'right'");

        winqEqual(left.like(right).escape("%"), "left LIKE 'right' ESCAPE '%'");

        winqEqual(left.glob(right).escape("%"), "left GLOB 'right' ESCAPE '%'");

        winqEqual(left.match(right).escape("%"), "left MATCH 'right' ESCAPE '%'");

        winqEqual(left.regexp(right).escape("%"), "left REGEXP 'right' ESCAPE '%'");

        winqEqual(left.notLike(right).escape("%"), "left NOT LIKE 'right' ESCAPE '%'");

        winqEqual(left.notGlob(right).escape("%"), "left NOT GLOB 'right' ESCAPE '%'");

        winqEqual(left.notMatch(right).escape("%"), "left NOT MATCH 'right' ESCAPE '%'");

        winqEqual(left.notRegexp(right).escape("%"), "left NOT REGEXP 'right' ESCAPE '%'");

        winqEqual(left.is(right), "left IS 'right'");

        winqEqual(left.isNot(right), "left IS NOT 'right'");

        winqEqual(left.avg(), "AVG(left)");

        winqEqual(left.count().distinct(), "COUNT(DISTINCT left)");

        winqEqual(left.groupConcat(), "GROUP_CONCAT(left)");

        winqEqual(left.groupConcat("-").distinct(),"GROUP_CONCAT(DISTINCT left, '-')");

        winqEqual(left.max(), "MAX(left)");

        winqEqual(left.min(), "MIN(left)");

        winqEqual(left.sum(), "SUM(left)");

        winqEqual(left.total(), "TOTAL(left)");

        winqEqual(left.abs(), "ABS(left)");

        winqEqual(left.hex(), "HEX(left)");

        winqEqual(left.length(), "LENGTH(left)");

        winqEqual(left.lower(), "LOWER(left)");

        winqEqual(left.upper(), "UPPER(left)");

        winqEqual(left.round(), "ROUND(left)");

        winqEqual(left.matchInfo(), "matchInfo(left)");

        winqEqual(left.offsets(), "offsets(left)");

        winqEqual(left.snippet(), "snippet(left)");

        winqEqual(left.bm25(), "bm25(left)");

        winqEqual(left.highlight(), "highlight(left)");

        winqEqual(left.substringMatchInfo(), "substring_match_info(left)");
    }
}
