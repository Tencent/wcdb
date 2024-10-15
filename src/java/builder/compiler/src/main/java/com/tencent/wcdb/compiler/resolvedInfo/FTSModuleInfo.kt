// Created by chenqiuwen on 2023/11/4.
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
import com.google.devtools.ksp.symbol.KSType
import com.google.devtools.ksp.symbol.KSTypeParameter
import com.tencent.wcdb.*

data class FTSModuleInfo(
    var ftsVersion: String = "",
    var tokenizer: String = "",
    var tokenizerParameters: MutableList<String> = mutableListOf(),
    var externalTable: String = "",
) {
    companion object {
        fun resolve(annotation: KSAnnotation?, logger: KSPLogger): FTSModuleInfo? {
            if (annotation == null) {
                return null
            }
            assert(annotation.shortName.asString() == "FTSModule")
            val resolvedInfo = FTSModuleInfo()
            for (argument in annotation.arguments) {
                val value = argument.value ?: continue
                when (argument.name?.asString()) {
                    "version" -> {
                        when ((value as KSType).declaration.toString()) {
                            FTSVersion.NONE.name -> resolvedInfo.ftsVersion = ""
                            FTSVersion.FTS3.name -> resolvedInfo.ftsVersion = "fts3"
                            FTSVersion.FTS4.name -> resolvedInfo.ftsVersion = "fts4"
                            FTSVersion.FTS5.name -> resolvedInfo.ftsVersion = "fts5"
                            else -> {
                                logger.error("Unrecognized FTSVersion ${value.declaration} in FTSModule")
                                return null
                            }
                        }
                    }

                    "tokenizer" -> resolvedInfo.tokenizer = value as String
                    "tokenizerParameters" -> {
                        if (!resolveValueToList(
                                value,
                                String::class,
                                resolvedInfo.tokenizerParameters
                            )
                        ) {
                            logger.error(
                                "Unresolved tokenizerParameters $value " +
                                        "with type ${value::class.qualifiedName} in FTSModule"
                            )
                            return null
                        }
                    }

                    "externalTable" -> resolvedInfo.externalTable = value as String
                    else -> {
                        logger.error("Unrecognized field ${argument.name?.asString()} in FTSModule")
                        return null
                    }
                }
            }
            return resolvedInfo
        }

        fun resolve(ftsModule: FTSModule?): FTSModuleInfo? {
            if (ftsModule == null) {
                return null
            }
            val resolvedInfo = FTSModuleInfo()
            when (ftsModule.version) {
                FTSVersion.NONE -> resolvedInfo.ftsVersion = ""
                FTSVersion.FTS3 -> resolvedInfo.ftsVersion = "fts3"
                FTSVersion.FTS4 -> resolvedInfo.ftsVersion = "fts4"
                FTSVersion.FTS5 -> resolvedInfo.ftsVersion = "fts5"
            }
            resolvedInfo.tokenizer = ftsModule.tokenizer
            resolvedInfo.tokenizerParameters = ftsModule.tokenizerParameters.toMutableList()
            resolvedInfo.externalTable = ftsModule.externalTable
            return resolvedInfo
        }
    }
}