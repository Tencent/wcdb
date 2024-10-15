// Created by qiuwenchen on 2023/3/30.
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
package com.tencent.wcdb.winq;

import com.tencent.wcdb.base.CppObject;
import com.tencent.wcdb.base.Value;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.List;
import java.util.Set;


public abstract class ExpressionOperable extends Identifier implements ExpressionConvertible {

    @NotNull
    public Expression isNull() {
        return nullOperate(false);
    }

    @NotNull
    public Expression notNull() {
        return nullOperate(true);
    }

    /*
        Binary operation
     */

    @NotNull
    public Expression or(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Or, false);
    }

    @NotNull
    public Expression and(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.And, false);
    }

    @NotNull
    public Expression multiply(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Multiply, false);
    }

    @NotNull
    public Expression multiply(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Multiply, false);
    }

    @NotNull
    public Expression multiply(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Multiply, false);
    }

    @NotNull
    public Expression multiply(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Multiply, false);
    }

    @NotNull
    public Expression multiply(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Multiply, false);
    }

    @NotNull
    public Expression multiply(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Multiply, false);
    }

    @NotNull
    public Expression multiply(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Multiply, false);
    }

    @NotNull
    public Expression divide(@NotNull ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Divide, false);
    }

    @NotNull
    public Expression divide(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Divide, false);
    }

    @NotNull
    public Expression divide(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Divide, false);
    }

    @NotNull
    public Expression divide(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Divide, false);
    }

    @NotNull
    public Expression divide(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Divide, false);
    }

    @NotNull
    public Expression divide(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Divide, false);
    }

    @NotNull
    public Expression divide(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Divide, false);
    }

    @NotNull
    public Expression mod(@NotNull ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Modulo, false);
    }

    @NotNull
    public Expression mod(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Modulo, false);
    }

    @NotNull
    public Expression mod(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Modulo, false);
    }

    @NotNull
    public Expression mod(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Modulo, false);
    }

    @NotNull
    public Expression mod(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Modulo, false);
    }

    @NotNull
    public Expression mod(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Modulo, false);
    }

    @NotNull
    public Expression mod(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Modulo, false);
    }

    @NotNull
    public Expression add(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Plus, false);
    }

    @NotNull
    public Expression add(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Plus, false);
    }

    @NotNull
    public Expression add(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Plus, false);
    }

    @NotNull
    public Expression add(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Plus, false);
    }

    @NotNull
    public Expression add(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Plus, false);
    }

    @NotNull
    public Expression add(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Plus, false);
    }

    @NotNull
    public Expression add(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Plus, false);
    }

    @NotNull
    public Expression minus(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Minus, false);
    }

    @NotNull
    public Expression minus(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Minus, false);
    }

    @NotNull
    public Expression minus(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Minus, false);
    }

    @NotNull
    public Expression minus(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Minus, false);
    }

    @NotNull
    public Expression minus(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Minus, false);
    }

    @NotNull
    public Expression minus(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Minus, false);
    }

    @NotNull
    public Expression minus(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Minus, false);
    }

    @NotNull
    public Expression leftShift(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.LeftShift, false);
    }

    @NotNull
    public Expression leftShift(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.LeftShift, false);
    }

    @NotNull
    public Expression leftShift(short operand) {
        return binaryOperate(operand, BinaryOperatorType.LeftShift, false);
    }

    @NotNull
    public Expression leftShift(int operand) {
        return binaryOperate(operand, BinaryOperatorType.LeftShift, false);
    }

    @NotNull
    public Expression leftShift(long operand) {
        return binaryOperate(operand, BinaryOperatorType.LeftShift, false);
    }

    @NotNull
    public Expression rightShift(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.RightShift, false);
    }

    @NotNull
    public Expression rightShift(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.RightShift, false);
    }

    @NotNull
    public Expression rightShift(short operand) {
        return binaryOperate(operand, BinaryOperatorType.RightShift, false);
    }

    @NotNull
    public Expression rightShift(int operand) {
        return binaryOperate(operand, BinaryOperatorType.RightShift, false);
    }

    @NotNull
    public Expression rightShift(long operand) {
        return binaryOperate(operand, BinaryOperatorType.RightShift, false);
    }

    @NotNull
    public Expression bitAnd(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseAnd, false);
    }

    @NotNull
    public Expression bitAnd(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseAnd, false);
    }

    @NotNull
    public Expression bitAnd(short operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseAnd, false);
    }

    @NotNull
    public Expression bitAnd(int operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseAnd, false);
    }

    @NotNull
    public Expression bitAnd(long operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseAnd, false);
    }

    @NotNull
    public Expression bitOr(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseOr, false);
    }

    @NotNull
    public Expression bitOr(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseOr, false);
    }

    @NotNull
    public Expression bitOr(short operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseOr, false);
    }

    @NotNull
    public Expression bitOr(int operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseOr, false);
    }

    @NotNull
    public Expression bitOr(long operand) {
        return binaryOperate(operand, BinaryOperatorType.BitwiseOr, false);
    }

    @NotNull
    public Expression lt(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Less, false);
    }

    @NotNull
    public Expression lt(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Less, false);
    }

    @NotNull
    public Expression lt(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Less, false);
    }

    @NotNull
    public Expression lt(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Less, false);
    }

    @NotNull
    public Expression lt(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Less, false);
    }

    @NotNull
    public Expression lt(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Less, false);
    }

    @NotNull
    public Expression lt(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.Less, false);
    }

    @NotNull
    public Expression le(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression le(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression le(short operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression le(int operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression le(long operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression le(float operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression le(double operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression le(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.LessOrEqual, false);
    }

    @NotNull
    public Expression gt(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression gt(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression gt(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression gt(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression gt(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression gt(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression gt(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression gt(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.Greater, false);
    }

    @NotNull
    public Expression ge(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression ge(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression ge(short operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression ge(int operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression ge(long operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression ge(float operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression ge(double operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression ge(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.GreaterOrEqual, false);
    }

    @NotNull
    public Expression eq(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(boolean operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression eq(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.Equal, false);
    }

    @NotNull
    public Expression notEq(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(boolean operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(short operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(int operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(long operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(float operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(double operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression notEq(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.NotEqual, false);
    }

    @NotNull
    public Expression concat(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    @NotNull
    public Expression concat(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    @NotNull
    public Expression concat(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    @NotNull
    public Expression concat(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    @NotNull
    public Expression concat(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    @NotNull
    public Expression concat(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    @NotNull
    public Expression concat(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    @NotNull
    public Expression concat(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Concatenate, false);
    }

    /*
        Between operation
     */

    @NotNull
    public Expression between(@Nullable ExpressionConvertible begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                false));
    }

    @NotNull
    public Expression between(@Nullable ExpressionConvertible begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                CPPType.Int, end, 0, null,
                false));
    }

    @NotNull
    public Expression between(@Nullable ExpressionConvertible begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                CPPType.Double, 0, end, null,
                false));
    }

    @NotNull
    public Expression between(@Nullable ExpressionConvertible begin, String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                CPPType.String, 0, 0, end,
                false));
    }

    @NotNull
    public Expression between(long begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                false));
    }

    @NotNull
    public Expression between(long begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                CPPType.Int, end, 0, null,
                false));
    }

    @NotNull
    public Expression between(long begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                CPPType.Double, 0, end, null,
                false));
    }

    @NotNull
    public Expression between(long begin, @Nullable String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                CPPType.String, 0, 0, end,
                false));
    }

    @NotNull
    public Expression between(double begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                false));
    }

    @NotNull
    public Expression between(double begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                CPPType.Int, end, 0, null,
                false));
    }

    @NotNull
    public Expression between(double begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                CPPType.Double, 0, end, null,
                false));
    }

    @NotNull
    public Expression between(double begin, @Nullable String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                CPPType.String, 0, 0, end,
                false));
    }

    @NotNull
    public Expression between(@Nullable String begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                false));
    }

    @NotNull
    public Expression between(@Nullable String begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                CPPType.Int, end, 0, null,
                false));
    }

    @NotNull
    public Expression between(@Nullable String begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                CPPType.Double, 0, end, null,
                false));
    }

    @NotNull
    public Expression between(@Nullable String begin, String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                CPPType.String, 0, 0, end,
                false));
    }

    @NotNull
    public Expression notBetween(@Nullable ExpressionConvertible begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(@Nullable ExpressionConvertible begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                CPPType.Int, end, 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(@Nullable ExpressionConvertible begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                CPPType.Double, 0, end, null,
                true));
    }

    @NotNull
    public Expression notBetween(@Nullable ExpressionConvertible begin, @Nullable String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                Identifier.getCppType(begin), CppObject.get(begin), 0, null,
                CPPType.String, 0, 0, end,
                true));
    }

    @NotNull
    public Expression notBetween(long begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(long begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                CPPType.Int, end, 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(long begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                CPPType.Double, 0, end, null,
                true));
    }

    @NotNull
    public Expression notBetween(long begin, @Nullable String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Int, begin, 0, null,
                CPPType.String, 0, 0, end,
                true));
    }

    @NotNull
    public Expression notBetween(double begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(double begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                CPPType.Int, end, 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(double begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                CPPType.Double, 0, end, null,
                true));
    }

    @NotNull
    public Expression notBetween(double begin, @Nullable String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.Double, 0, begin, null,
                CPPType.String, 0, 0, end,
                true));
    }

    @NotNull
    public Expression notBetween(@Nullable String begin, @Nullable ExpressionConvertible end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                Identifier.getCppType(end), CppObject.get(end), 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(@Nullable String begin, long end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                CPPType.Int, end, 0, null,
                true));
    }

    @NotNull
    public Expression notBetween(@Nullable String begin, double end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                CPPType.Double, 0, end, null,
                true));
    }

    @NotNull
    public Expression notBetween(@Nullable String begin, @Nullable String end) {
        return createExpression(betweenOperate(Identifier.getCppType(this), CppObject.get(this),
                CPPType.String, 0, 0, begin,
                CPPType.String, 0, 0, end,
                true));
    }

    /*
        In operation
     */

    @NotNull
    public Expression in(short... operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, false);
    }

    @NotNull
    public Expression in(@NotNull Short[] operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, false);
    }

    @NotNull
    public Expression in(int... operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, false);
    }

    @NotNull
    public Expression in(@NotNull Integer[] operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, false);
    }

    @NotNull
    public Expression in(long... operands) {
        return in(operands, false);
    }

    @NotNull
    public Expression in(@NotNull Long[] operands) {
        return in(operands, false);
    }

    @NotNull
    public Expression in(float... operands) {
        double[] values = new double[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, false);
    }

    @NotNull
    public Expression in(@NotNull Float[] operands) {
        double[] values = new double[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, false);
    }

    @NotNull
    public Expression in(double... operands) {
        return in(operands, false);
    }

    @NotNull
    public Expression in(@NotNull Double[] operands) {
        return in(operands, false);
    }

    @NotNull
    public Expression in(@NotNull String... operands) {
        return in(operands, false);
    }

    @NotNull
    public Expression in(@NotNull Value... operands) {
        return in(operands, false);
    }

    @NotNull
    public <T> Expression in(@NotNull Set<T> operands) {
        return in(operands.toArray(), false);
    }

    @NotNull
    public <T> Expression in(@NotNull List<T> operands) {
        return in(operands.toArray(), false);
    }

    @NotNull
    public Expression notIn(short... operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, true);
    }

    @NotNull
    public Expression notIn(@NotNull Short[] operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, true);
    }

    @NotNull
    public Expression notIn(int... operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, true);
    }

    @NotNull
    public Expression notIn(@NotNull Integer[] operands) {
        long[] values = new long[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, true);
    }

    @NotNull
    public Expression notIn(long... operands) {
        return in(operands, true);
    }

    @NotNull
    public Expression notIn(@NotNull Long[] operands) {
        return in(operands, true);
    }

    @NotNull
    public Expression notIn(float... operands) {
        double[] values = new double[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, true);
    }

    @NotNull
    public Expression notIn(@NotNull Float[] operands) {
        double[] values = new double[operands.length];
        for (int i = 0; i < operands.length; i++) {
            values[i] = operands[i];
        }
        return in(values, true);
    }

    @NotNull
    public Expression notIn(double... operands) {
        return in(operands, true);
    }

    @NotNull
    public Expression notIn(@NotNull Double[] operands) {
        return in(operands, true);
    }

    @NotNull
    public Expression notIn(@NotNull String... operands) {
        return in(operands, true);
    }

    @NotNull
    public Expression notIn(@NotNull Value... operands) {
        return in(operands, true);
    }

    @NotNull
    public <T> Expression notIn(@NotNull Set<T> operands) {
        return in(operands.toArray(), true);
    }

    @NotNull
    public <T> Expression notIn(@NotNull List<T> operands) {
        return in(operands.toArray(), true);
    }

    @NotNull
    public Expression inTable(@NotNull String table) {
        return inTable(table, false);
    }

    @NotNull
    public Expression notInTable(@NotNull String table) {
        return inTable(table, true);
    }

    @NotNull
    public Expression inFunction(@NotNull String table) {
        return inFunction(table, false);
    }

    @NotNull
    public Expression notInFunction(@NotNull String table) {
        return inFunction(table, true);
    }

    @NotNull
    public Expression in(@NotNull StatementSelect select) {
        return in(select, false);
    }

    @NotNull
    public Expression notIn(@NotNull StatementSelect select) {
        return in(select, true);
    }

    /*
        Collate
     */

    @NotNull
    public Expression collate(@NotNull String collation) {
        return createExpression(collate(Identifier.getCppType(this), CppObject.get(this), collation));
    }

    /*
        Function
     */

    @NotNull
    public Expression substr(short start, short length) {
        return Expression.function("SUBSTR")
                .argument(this)
                .argument(start)
                .argument(length);
    }

    @NotNull
    public Expression substr(int start, int length) {
        return Expression.function("SUBSTR")
                .argument(this)
                .argument(start)
                .argument(length);
    }

    @NotNull
    public Expression substr(long start, long length) {
        return Expression.function("SUBSTR")
                .argument(this)
                .argument(start)
                .argument(length);
    }

    @NotNull
    public Expression like(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.Like, false);
    }

    @NotNull
    public Expression notLike(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.Like, true);
    }

    @NotNull
    public Expression glob(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.GLOB, false);
    }

    @NotNull
    public Expression notGlob(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.GLOB, true);
    }

    @NotNull
    public Expression match(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.Match, false);
    }

    @NotNull
    public Expression notMatch(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.Match, true);
    }

    @NotNull
    public Expression regexp(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.RegExp, false);
    }

    @NotNull
    public Expression notRegexp(@NotNull String content) {
        return binaryOperate(content, BinaryOperatorType.RegExp, true);
    }

    @NotNull
    public Expression is(boolean operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression is(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, false);
    }

    @NotNull
    public Expression isNot(boolean operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(byte operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(short operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(int operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(long operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(float operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(double operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(@Nullable String operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression isNot(@Nullable ExpressionConvertible operand) {
        return binaryOperate(operand, BinaryOperatorType.Is, true);
    }

    @NotNull
    public Expression avg() {
        return Expression.function("AVG").argument(this);
    }

    @NotNull
    public Expression count() {
        return Expression.function("COUNT").argument(this);
    }

    @NotNull
    public Expression groupConcat() {
        return Expression.function("GROUP_CONCAT").argument(this);
    }

    @NotNull
    public Expression groupConcat(String sperator) {
        return Expression.function("GROUP_CONCAT").argument(this).argument(sperator);
    }

    @NotNull
    public Expression max() {
        return Expression.function("MAX").argument(this);
    }

    @NotNull
    public Expression min() {
        return Expression.function("MIN").argument(this);
    }

    @NotNull
    public Expression sum() {
        return Expression.function("SUM").argument(this);
    }

    @NotNull
    public Expression total() {
        return Expression.function("TOTAL").argument(this);
    }

    @NotNull
    public Expression abs() {
        return Expression.function("ABS").argument(this);
    }

    @NotNull
    public Expression hex() {
        return Expression.function("HEX").argument(this);
    }

    @NotNull
    public Expression length() {
        return Expression.function("LENGTH").argument(this);
    }

    @NotNull
    public Expression lower() {
        return Expression.function("LOWER").argument(this);
    }

    @NotNull
    public Expression upper() {
        return Expression.function("UPPER").argument(this);
    }

    @NotNull
    public Expression round() {
        return Expression.function("ROUND").argument(this);
    }

    @NotNull
    public Expression matchInfo() {
        return Expression.function("matchInfo").argument(this);
    }

    @NotNull
    public Expression offsets() {
        return Expression.function("offsets").argument(this);
    }

    @NotNull
    public Expression snippet() {
        return Expression.function("snippet").argument(this);
    }

    @NotNull
    public Expression bm25() {
        return Expression.function("bm25").argument(this);
    }

    @NotNull
    public Expression highlight() {
        return Expression.function("highlight").argument(this);
    }

    @NotNull
    public Expression substringMatchInfo() {
        return Expression.function("substring_match_info").argument(this);
    }

    /*
        Private & native
    */

    protected static Expression createExpression(long cppObj) {
        Expression ret = new Expression();
        ret.cppObj = cppObj;
        return ret;
    }

    private Expression nullOperate(boolean isNot) {
        Expression ret = new Expression();
        ret.cppObj = nullOperate(Identifier.getCppType(this), CppObject.get(this), isNot);
        return ret;
    }

    private static native long nullOperate(int type, long operand, boolean isNot);

    private static class BinaryOperatorType {
        final static int Concatenate = 1;
        final static int Multiply = 2;
        final static int Divide = 3;
        final static int Modulo = 4;
        final static int Plus = 5;
        final static int Minus = 6;
        final static int LeftShift = 7;
        final static int RightShift = 8;
        final static int BitwiseAnd = 9;
        final static int BitwiseOr = 10;
        final static int Less = 11;
        final static int LessOrEqual = 12;
        final static int Greater = 13;
        final static int GreaterOrEqual = 14;
        final static int Equal = 15;
        final static int NotEqual = 16;
        final static int Is = 17;
        final static int And = 18;
        final static int Or = 19;
        final static int Like = 20;
        final static int GLOB = 21;
        final static int RegExp = 22;
        final static int Match = 23;
    }

    private Expression binaryOperate(ExpressionConvertible operand, int type, boolean isNot) {
        return createExpression(binaryOperate(Identifier.getCppType(this),
                CppObject.get(this),
                Identifier.getCppType(operand),
                CppObject.get(operand),
                0,
                null,
                type,
                isNot));
    }

    private Expression binaryOperate(boolean operand, int type, boolean isNot) {
        return createExpression(binaryOperate(Identifier.getCppType(this),
                CppObject.get(this),
                CPPType.Bool,
                operand ? 1 : 0,
                0,
                null,
                type,
                isNot));
    }

    private Expression binaryOperate(long operand, int type, boolean isNot) {
        return createExpression(binaryOperate(Identifier.getCppType(this),
                CppObject.get(this),
                CPPType.Int,
                operand,
                0,
                null,
                type,
                isNot));
    }

    private Expression binaryOperate(double operand, int type, boolean isNot) {
        return createExpression(binaryOperate(Identifier.getCppType(this),
                CppObject.get(this),
                CPPType.Double,
                0,
                operand,
                null,
                type,
                isNot));
    }

    private Expression binaryOperate(String operand, int type, boolean isNot) {
        return createExpression(binaryOperate(Identifier.getCppType(this),
                CppObject.get(this),
                CPPType.String,
                0,
                0,
                operand,
                type,
                isNot));
    }

    private static native long binaryOperate(int leftType, long left,
                                             int rightType, long right_long, double right_double, String right_string,
                                             int operatorType,
                                             boolean isNot);

    private static native long betweenOperate(int operandType, long operand,
                                              int leftType, long left_long, double left_double, String left_string,
                                              int rightType, long right_long, double right_double, String right_string,
                                              boolean isNot);

    private Expression in(long[] operands, boolean isNot) {
        return createExpression(in(Identifier.getCppType(this),
                CppObject.get(this),
                CPPType.Int,
                operands,
                null,
                null,
                isNot));
    }

    private Expression in(int type, long[] operands, boolean isNot) {
        return createExpression(in(Identifier.getCppType(this),
                CppObject.get(this),
                type,
                operands,
                null,
                null,
                isNot));
    }

    private Expression in(double[] operands, boolean isNot) {
        return createExpression(in(Identifier.getCppType(this),
                CppObject.get(this),
                CPPType.Double,
                null,
                operands,
                null,
                isNot));
    }

    private Expression in(String[] operands, boolean isNot) {
        return createExpression(in(Identifier.getCppType(this),
                CppObject.get(this),
                CPPType.String,
                null,
                null,
                operands,
                isNot));
    }

    private static native long in(int operandType, long operand, int type, long[] longArray, double[] doubleArray, String[] stringArray, boolean isNot);

    private Expression in(Object[] operands, boolean isNot) {
        if (operands == null || operands.length == 0) {
            return in((long[]) null, isNot);
        }

        int dataType = MultiTypeArray.getObjectType(operands[0]);
        if (dataType == ObjectType.Identifier) {
            long[] cppObjs = new long[operands.length];
            for (int i = 0; i < operands.length; i++) {
                cppObjs[i] = CppObject.get((Identifier) operands[i]);
            }
            return in(Identifier.getCppType((Identifier) operands[0]), cppObjs, isNot);
        } else if (dataType == ObjectType.Value) {
            switch (((Value) operands[0]).getType()) {
                case Integer:
                    long[] intArray = new long[operands.length];
                    for (int i = 0; i < operands.length; i++) {
                        intArray[i] = ((Value) operands[i]).getLong();
                    }
                    return in(intArray, isNot);
                case Float:
                    double[] doubleArray = new double[operands.length];
                    for (int i = 0; i < operands.length; i++) {
                        doubleArray[i] = ((Value) operands[i]).getDouble();
                    }
                    return in(doubleArray, isNot);
                case Text:
                    String[] textArray = new String[operands.length];
                    for (int i = 0; i < operands.length; i++) {
                        textArray[i] = ((Value) operands[i]).getText();
                    }
                    return in(textArray, isNot);
            }
            if (operands instanceof String[]) {
                return in((String[]) operands, isNot);
            } else {
                String[] stringArray = new String[operands.length];
                for (int i = 0; i < stringArray.length; i++) {
                    stringArray[i] = (String) operands[i];
                }
                return in(stringArray, isNot);
            }
        } else if (dataType == ObjectType.String) {
            if (operands instanceof String[]) {
                return in((String[]) operands, isNot);
            } else {
                String[] stringArray = new String[operands.length];
                for (int i = 0; i < stringArray.length; i++) {
                    stringArray[i] = (String) operands[i];
                }
                return in(stringArray, isNot);
            }
        } else if (dataType < ObjectType.Float) {
            long[] values = new long[operands.length];
            for (int i = 0; i < operands.length; i++) {
                switch (dataType) {
                    case ObjectType.Null:
                        values[i] = 0;
                        break;
                    case ObjectType.Bool:
                        values[i] = (Boolean) operands[i] ? 1 : 0;
                        break;
                    case ObjectType.Char:
                        values[i] = (Character) operands[i];
                        break;
                    case ObjectType.Short:
                        values[i] = (Short) operands[i];
                        break;
                    case ObjectType.Int:
                        values[i] = (Integer) operands[i];
                        break;
                    case ObjectType.Long:
                        values[i] = (Long) operands[i];
                        break;
                }
            }
            return in(values, isNot);
        } else if (dataType != ObjectType.Unknown) {
            double[] values = new double[operands.length];
            for (int i = 0; i < operands.length; i++) {
                if (dataType == ObjectType.Float) {
                    values[i] = (Float) operands[i];
                } else {
                    values[i] = (Double) operands[i];
                }
            }
            return in(values, isNot);
        }
        return in((long[]) null);
    }

    private Expression inTable(String table, boolean isNot) {
        return createExpression(inTable(Identifier.getCppType(this), CppObject.get(this), table, isNot));
    }

    private static native long inTable(int type, long operand, String table, boolean isNot);

    private Expression inFunction(String func, boolean isNot) {
        return createExpression(inFunction(Identifier.getCppType(this), CppObject.get(this), func, isNot));
    }

    private static native long inFunction(int type, long operand, String func, boolean isNot);

    private Expression in(StatementSelect select, boolean isNot) {
        return createExpression(inSelect(Identifier.getCppType(this), CppObject.get(this), CppObject.get(select), isNot));
    }

    private static native long inSelect(int type, long operand, long select, boolean isNot);

    private static native long collate(int type, long operand, String collation);
}
