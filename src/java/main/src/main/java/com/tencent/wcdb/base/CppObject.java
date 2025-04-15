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
package com.tencent.wcdb.base;

public class CppObject implements CppObjectConvertible {
    static {
        System.loadLibrary("WCDB");
    }

    @Override
    protected void finalize() throws Throwable {
        long cObj = cppObj;
        cppObj = 0;
        if (cObj != 0) {
            releaseCPPObject(cObj);
        }
        super.finalize();
    }

    protected long cppObj = 0;

    public static long get(CppObject obj) {
        if (obj == null) {
            return 0;
        }
        return obj.cppObj;
    }

    public static long get(CppObjectConvertible obj) {
        if (obj == null) {
            return 0;
        }
        return obj.asCppObject().cppObj;
    }

    protected static native void releaseCPPObject(long obj);

    @Override
    public CppObject asCppObject() {
        return this;
    }
}
