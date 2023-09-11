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
import com.tencent.wcdb.WCDBTableCoding

data class TableConstraintInfo(
    var multiIndexes: MutableList<MultiIndexesInfo> = mutableListOf(),
    var multiPrimaries: MutableList<MultiPrimaryInfo> = mutableListOf(),
    var multiUnique: MutableList<MultiUniqueInfo> = mutableListOf(),
    var isWithoutRowId: Boolean = false
) {
    companion object {
        fun resolve(annotation: KSAnnotation?, logger: KSPLogger): TableConstraintInfo? {
            if(annotation == null) {
                return null
            }
            val resolvedAnnotation = TableConstraintInfo()
            for(argument in annotation.arguments) {
                val value = argument.value ?: continue
                val annotationList = mutableListOf<KSAnnotation>()
                when (argument.name?.asString()) {
                    "multiIndexes" -> {
                        if(!resolveValueToList(value, KSAnnotation::class, annotationList)) {
                            logger.error("Unresolved multiIndexes $value " +
                                    "with type ${value::class.qualifiedName} in WCDBTableCoding")
                            return null
                        }
                        for(subAnnotation in annotationList) {
                            val multiIndexes = MultiIndexesInfo.resolve(subAnnotation, logger) ?: return null
                            resolvedAnnotation.multiIndexes.add(multiIndexes)
                        }
                    }
                    "multiPrimaries" -> {
                        if(!resolveValueToList(value, KSAnnotation::class, annotationList)) {
                            logger.error("Unresolved multiPrimaries $value " +
                                    "with type ${value::class.qualifiedName} in WCDBTableCoding")
                            return null
                        }
                        for(subAnnotation in annotationList) {
                            val multiPrimaries = MultiPrimaryInfo.resolve(subAnnotation, logger) ?: return null
                            resolvedAnnotation.multiPrimaries.add(multiPrimaries)
                        }
                    }
                    "multiUnique" -> {
                        if(!resolveValueToList(value, KSAnnotation::class, annotationList)) {
                            logger.error("Unresolved multiUnique $value " +
                                    "with type ${value::class.qualifiedName} in WCDBTableCoding")
                            return null
                        }
                        for(subAnnotation in annotationList) {
                            val multiUnique = MultiUniqueInfo.resolve(subAnnotation, logger) ?: return null
                            resolvedAnnotation.multiUnique.add(multiUnique)
                        }
                    }
                    "isWithoutRowId" -> {
                        resolvedAnnotation.isWithoutRowId = value as Boolean
                    }
                    else -> {
                        logger.error("Unrecognized field ${argument.name?.asString()} in WCDBTableCoding")
                        return null
                    }
                }
            }
            return resolvedAnnotation
        }

        fun resolve(tableCoding: WCDBTableCoding): TableConstraintInfo {
            val resolvedAnnotation = TableConstraintInfo()
            resolvedAnnotation.isWithoutRowId = tableCoding.isWithoutRowId
            for(multiIndexes in tableCoding.multiIndexes) {
                resolvedAnnotation.multiIndexes.add(MultiIndexesInfo.resolve(multiIndexes))
            }
            for(multiPrimary in tableCoding.multiPrimaries) {
                resolvedAnnotation.multiPrimaries.add(MultiPrimaryInfo.resolve(multiPrimary))
            }
            for(multiUnique in tableCoding.multiUnique) {
                resolvedAnnotation.multiUnique.add(MultiUniqueInfo.resolve(multiUnique))
            }
            return resolvedAnnotation
        }
    }
}