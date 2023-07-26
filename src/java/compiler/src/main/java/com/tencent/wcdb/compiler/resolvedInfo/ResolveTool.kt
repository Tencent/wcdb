// Created by qiuwenchen on 2023/6/21.
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

package com.tencent.wcdb.compiler.resolvedInfo

import kotlin.reflect.KClass

internal fun <T : Any> resolveValueToList(value: Any, kClass: KClass<T>, list: MutableList<T>): Boolean {
    if(kClass.isInstance(value)) {
        @Suppress("UNCHECKED_CAST")
        list.add(value as T)
    } else if(value is ArrayList<*>) {
        @Suppress("UNCHECKED_CAST")
        val array = value as ArrayList<T>
        if(array.isNotEmpty()) {
            list.addAll(array)
        }
    } else {
        return false
    }
    return true
}