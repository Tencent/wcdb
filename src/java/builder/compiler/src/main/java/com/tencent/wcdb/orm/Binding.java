// Created by chenqiuwen on 2023/4/19.
//

/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the &quot;License&quot;); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an &quot;AS IS&quot; BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tencent.wcdb.orm;

import com.tencent.wcdb.winq.ColumnDef;
import com.tencent.wcdb.winq.StatementCreateIndex;
import com.tencent.wcdb.winq.TableConstraint;

public class Binding {
    public Binding() { throw new UnsatisfiedLinkError(); }
    public void addColumnDef(ColumnDef def) { throw new UnsatisfiedLinkError(); }
    public void addIndex(String indexPrefix, StatementCreateIndex createIndex) { throw new UnsatisfiedLinkError(); }
    public void addTableConstraint(TableConstraint constraint) { throw new UnsatisfiedLinkError(); }
    public Binding configVirtualModule(String module) { throw new UnsatisfiedLinkError(); }
    public void configVirtualModuleArgument(String argument) { throw new UnsatisfiedLinkError(); }
}
