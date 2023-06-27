// Created by qiuwenchen on 2023/6/27.
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
import com.tencent.wcdb.WCDBDefault
import javax.swing.LookAndFeel

data class DefaultValueInfo(
    var intValue: Long = 0,
    var doubleValue: Double = 0.0,
    var textValue: String = "",
){
    companion object {
        fun resolve(defaultValueAnnotation: KSAnnotation, logger: KSPLogger): DefaultValueInfo?{
            val resolvedInfo = DefaultValueInfo()
            for(argument in defaultValueAnnotation.arguments) {
                val value = argument.value ?: continue
                when (argument.name?.asString()) {
                    "intValue" -> {
                        if (value is Int) {
                            resolvedInfo.intValue = value.toLong()
                        } else if (value is Long) {
                            resolvedInfo.intValue = value
                        } else {
                            logger.error("Unrecognized int value type ${value::class} in WCDBDefault")
                            return null
                        }
                    }
                    "doubleValue" -> resolvedInfo.doubleValue = value as Double
                    "textValue" -> resolvedInfo.textValue = value as String
                    else -> {
                        logger.error("Unrecognized field ${argument.name?.asString()} in WCDBDefault")
                        return null
                    }
                }
            }
            return resolvedInfo
        }

        fun resolve(defaultValue: WCDBDefault): DefaultValueInfo {
            val resolvedInfo = DefaultValueInfo()
            resolvedInfo.intValue = defaultValue.intValue
            resolvedInfo.doubleValue = defaultValue.doubleValue
            resolvedInfo.textValue = defaultValue.textValue
            return resolvedInfo
        }
    }
}