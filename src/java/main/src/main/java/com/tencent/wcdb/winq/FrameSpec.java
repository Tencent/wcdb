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

public class FrameSpec extends Identifier {
    @Override
    protected CPPType getType() {
        return CPPType.FrameSpec;
    }

    public FrameSpec() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    public FrameSpec range() {
        configRange(cppObj);
        return this;
    }

    private static native void configRange(long self);

    public FrameSpec rows() {
        configRows(cppObj);
        return this;
    }

    private static native void configRows(long self);

    public FrameSpec unboundedPreceding() {
        configUnboundedPreceding(cppObj);
        return this;
    }

    private static native void configUnboundedPreceding(long self);

    public FrameSpec preceding(long offset) {
        configPreceding(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public FrameSpec preceding(ExpressionConvertible offset) {
        configPreceding(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configPreceding(long self, int type, long expression);

    public FrameSpec currentRow() {
        configCurrentRow(cppObj);
        return this;
    }

    private static native void configCurrentRow(long self);

    public FrameSpec betweenUnboundedPreceding() {
        configBetweenUnboundedPreceding(cppObj);
        return this;
    }

    private static native void configBetweenUnboundedPreceding(long self);

    public FrameSpec betweenPreceding(long offset) {
        configBetweenPreceding(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public FrameSpec betweenPreceding(ExpressionConvertible offset) {
        configBetweenPreceding(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configBetweenPreceding(long self, int type, long expression);

    public FrameSpec betweenCurrentRow() {
        configBetweenCurrentRow(cppObj);
        return this;
    }

    private static native void configBetweenCurrentRow(long self);

    public FrameSpec betweenFollowing(long offset) {
        configBetweenFollowing(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public FrameSpec betweenFollowing(ExpressionConvertible offset) {
        configBetweenFollowing(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configBetweenFollowing(long self, int type, long expression);

    public FrameSpec andCurrentRow() {
        configAndCurrentRow(cppObj);
        return this;
    }

    private static native void configAndCurrentRow(long self);

    public FrameSpec andPreceding(long offset) {
        configAndPreceding(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public FrameSpec andPreceding(ExpressionConvertible offset) {
        configAndPreceding(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configAndPreceding(long self, int type, long expression);

    public FrameSpec andUnboundedFollowing() {
        configAndUnboundedFollowing(cppObj);
        return this;
    }

    private static native void configAndUnboundedFollowing(long self);

    public FrameSpec andFollowing(long offset) {
        configAndFollowing(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public FrameSpec andFollowing(ExpressionConvertible offset) {
        configAndFollowing(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private native void configAndFollowing(long self, int type, long expression);
}
