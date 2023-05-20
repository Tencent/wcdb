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
package com.tencent.wcdb.orm;

import com.tencent.wcdb.core.PreparedStatement;

public interface TableBinding<T> {
    Class<T> bindingType();

    Field<T>[] allBindingFields();

    Binding baseBinding();

    T extractObject(Field<T>[] fields, PreparedStatement preparedStatement);

    void bindField(T object, Field<T> field, int index, PreparedStatement preparedStatement);

    boolean isAutoIncrement(T object);

    void setLastInsertRowId(T object, long lastInsertRowId);
}
