// Created by qiuwenchen on 2023/3/31.
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

import org.jetbrains.annotations.NotNull;

public class BindParameter extends Identifier implements ExpressionConvertible {
    @Override
    protected int getType() {
        return CPPType.BindParameter;
    }

    @NotNull
    @Override
    public Identifier asIdentifier() {
        return this;
    }

    public BindParameter(int num) {
        cppObj = createCppObj(num);
    }

    private BindParameter() {
    }

    private static native long createCppObj(int num);

    public BindParameter(@NotNull String name) {
        cppObj = createCppObj(name);
    }

    private static native long createCppObj(String name);

    public static BindParameter at(@NotNull String name) {
        BindParameter ret = new BindParameter();
        ret.cppObj = atBindParameter(name);
        return ret;
    }

    private static native long atBindParameter(String name);

    public static BindParameter colon(@NotNull String name) {
        return new BindParameter(name);
    }

    public static BindParameter dollar(@NotNull String name) {
        BindParameter ret = new BindParameter();
        ret.cppObj = dollarBindParameter(name);
        return ret;
    }

    private static native long dollarBindParameter(String name);

    public static BindParameter[] bindParameters(int num) {
        if (num == 0) {
            return new BindParameter[]{};
        }
        BindParameter[] bindParameters = new BindParameter[num];
        long[] cppObjs = createCppObjs(num);
        for (int i = 0; i < num; i++) {
            bindParameters[i] = new BindParameter();
            bindParameters[i].cppObj = cppObjs[i];
        }
        return bindParameters;
    }

    private static native long[] createCppObjs(int num);

    public static final BindParameter def = new BindParameter(0); //?
    public static final BindParameter _1 = new BindParameter(1); //?1
    public static final BindParameter _2 = new BindParameter(2); //?2
    public static final BindParameter _3 = new BindParameter(3); //?3
    public static final BindParameter _4 = new BindParameter(4); //?4
    public static final BindParameter _5 = new BindParameter(5); //?5
    public static final BindParameter _6 = new BindParameter(6); //?6
    public static final BindParameter _7 = new BindParameter(7); //?7
    public static final BindParameter _8 = new BindParameter(8); //?8
    public static final BindParameter _9 = new BindParameter(9); //?9
    public static final BindParameter _10 = new BindParameter(10); //?10

}
