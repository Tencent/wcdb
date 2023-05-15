// Created by qiuwenchen on 2023/5/8.
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

package com.tencent.wcdbtest.base;

import java.io.File;
import java.io.IOException;

public class FileTool {
    public static void createFile(String path) throws IOException {
        File file = new File(path);
        if(file.exists()) {
            return;
        }
        if(!file.getParentFile().exists()) {
            file.getParentFile().mkdirs();
        }
        file.createNewFile();
    }

    public static boolean fileExist(String path) {
        File file = new File(path);
        return file.exists();
    }

    public static long getFileSize(String path) {
        File file = new File(path);
        if(file.exists()) {
            return file.length();
        } else {
            return 0;
        }
    }
}
