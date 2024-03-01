// Created by qiuwenchen on 2023/6/9.
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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class FrameSpec extends Identifier {
    @Override
    protected int getType() {
        return CPPType.FrameSpec;
    }

    public FrameSpec() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public FrameSpec range() {
        configRange(cppObj);
        return this;
    }

    private static native void configRange(long self);

    @NotNull
    public FrameSpec rows() {
        configRows(cppObj);
        return this;
    }

    private static native void configRows(long self);

    @NotNull
    public FrameSpec unboundedPreceding() {
        configUnboundedPreceding(cppObj);
        return this;
    }

    private static native void configUnboundedPreceding(long self);

    @NotNull
    public FrameSpec preceding(long offset) {
        configPreceding(cppObj, CPPType.Int, offset);
        return this;
    }

    @NotNull
    public FrameSpec preceding(@Nullable ExpressionConvertible offset) {
        configPreceding(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configPreceding(long self, int type, long expression);

    @NotNull
    public FrameSpec currentRow() {
        configCurrentRow(cppObj);
        return this;
    }

    private static native void configCurrentRow(long self);

    @NotNull
    public FrameSpec betweenUnboundedPreceding() {
        configBetweenUnboundedPreceding(cppObj);
        return this;
    }

    private static native void configBetweenUnboundedPreceding(long self);

    @NotNull
    public FrameSpec betweenPreceding(long offset) {
        configBetweenPreceding(cppObj, CPPType.Int, offset);
        return this;
    }

    @NotNull
    public FrameSpec betweenPreceding(@Nullable ExpressionConvertible offset) {
        configBetweenPreceding(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configBetweenPreceding(long self, int type, long expression);

    @NotNull
    public FrameSpec betweenCurrentRow() {
        configBetweenCurrentRow(cppObj);
        return this;
    }

    private static native void configBetweenCurrentRow(long self);

    @NotNull
    public FrameSpec betweenFollowing(long offset) {
        configBetweenFollowing(cppObj, CPPType.Int, offset);
        return this;
    }

    @NotNull
    public FrameSpec betweenFollowing(@Nullable ExpressionConvertible offset) {
        configBetweenFollowing(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configBetweenFollowing(long self, int type, long expression);

    @NotNull
    public FrameSpec andCurrentRow() {
        configAndCurrentRow(cppObj);
        return this;
    }

    private static native void configAndCurrentRow(long self);

    @NotNull
    public FrameSpec andPreceding(long offset) {
        configAndPreceding(cppObj, CPPType.Int, offset);
        return this;
    }

    @NotNull
    public FrameSpec andPreceding(@Nullable ExpressionConvertible offset) {
        configAndPreceding(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configAndPreceding(long self, int type, long expression);

    @NotNull
    public FrameSpec andUnboundedFollowing() {
        configAndUnboundedFollowing(cppObj);
        return this;
    }

    private static native void configAndUnboundedFollowing(long self);

    @NotNull
    public FrameSpec andFollowing(long offset) {
        configAndFollowing(cppObj, CPPType.Int, offset);
        return this;
    }

    @NotNull
    public FrameSpec andFollowing(@Nullable ExpressionConvertible offset) {
        configAndFollowing(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configAndFollowing(long self, int type, long expression);
}
