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

import com.google.devtools.ksp.processing.KSPLogger
import com.google.devtools.ksp.symbol.KSAnnotation
import com.tencent.wcdb.MultiPrimary

data class MultiPrimaryInfo(
    var columns: MutableList<String> = mutableListOf()
) {
    companion object {
        fun resolve(annotation: KSAnnotation?, logger: KSPLogger): MultiPrimaryInfo?{
            if(annotation == null) {
                return null
            }
            assert(annotation.shortName.asString() == "MultiPrimary")
            val resolvedInfo = MultiPrimaryInfo()
            for(argument in annotation.arguments) {
                val value = argument.value ?: continue
                when (argument.name?.asString()) {
                    "columns" -> {
                        if(!resolveValueToList(value, String::class, resolvedInfo.columns)) {
                            logger.error("Unresolved columns $value " +
                                    "with type ${value::class.qualifiedName} in MultiPrimary")
                            return null
                        }
                    }
                    else -> {
                        logger.error("Unrecognized field ${argument.name?.asString()} in MultiPrimary")
                        return null
                    }
                }
            }
            return resolvedInfo
        }

        fun resolve(multiPrimary: MultiPrimary): MultiPrimaryInfo {
            val resolvedInfo = MultiPrimaryInfo()
            resolvedInfo.columns.addAll(multiPrimary.columns)
            return resolvedInfo
        }
    }
}