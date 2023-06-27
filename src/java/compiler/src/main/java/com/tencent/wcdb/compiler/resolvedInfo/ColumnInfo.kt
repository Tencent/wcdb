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
import com.google.devtools.ksp.symbol.KSPropertyDeclaration
import com.tencent.wcdb.WCDBDefault
import com.tencent.wcdb.WCDBField
import com.tencent.wcdb.WCDBIndex
import javax.lang.model.element.Element

data class ColumnInfo(
    var propertyName: String = "",
    var propertyType: String = "",
    var columnName: String = "",
    var isPrimary: Boolean = false,
    var isAutoIncrement: Boolean = false,
    var defaultValue: DefaultValueInfo? = null,
    var isUnique: Boolean = false,
    var isNotNull: Boolean = false,
    var indexNameSuffix: String = "",
    var indexIsUnique: Boolean = false,
) {
    companion object {
        fun resolve(propertyDeclaration: KSPropertyDeclaration,
                    fieldAnnotation: KSAnnotation,
                    indexAnnotation: KSAnnotation?,
                    defaultValueAnnotation: KSAnnotation?,
                    logger: KSPLogger): ColumnInfo?{
            if(fieldAnnotation == null) {
                return null
            }
            val resolvedInfo = ColumnInfo()
            resolvedInfo.propertyName = propertyDeclaration.simpleName.asString()
            resolvedInfo.propertyType = propertyDeclaration.type.resolve().declaration.qualifiedName!!.asString()
            for(argument in fieldAnnotation.arguments) {
                val value = argument.value ?: continue
                when (argument.name?.asString()) {
                    "columnName" -> resolvedInfo.columnName = value as String
                    "isPrimary" -> resolvedInfo.isPrimary = value as Boolean
                    "isAutoIncrement" -> resolvedInfo.isAutoIncrement = value as Boolean
                    "isUnique" -> resolvedInfo.isUnique = value as Boolean
                    "isNotNull" -> resolvedInfo.isNotNull = value as Boolean
                    else -> {
                        logger.error("Unrecognized field ${argument.name?.asString()} in WCDBField")
                        return null
                    }
                }
            }
            if(indexAnnotation != null) {
                for(argument in indexAnnotation.arguments) {
                    val value = argument.value ?: continue
                    when (argument.name?.asString()) {
                        "nameSuffix" -> resolvedInfo.indexNameSuffix = value as String
                        "isUnique" -> resolvedInfo.indexIsUnique = value as Boolean
                        else -> {
                            logger.error("Unrecognized field ${argument.name?.asString()} in WCDBIndex")
                            return null
                        }
                    }
                }
            }
            if(defaultValueAnnotation != null) {
                resolvedInfo.defaultValue = DefaultValueInfo.resolve(defaultValueAnnotation, logger);
            }

            return resolvedInfo
        }

        fun resolve(fieldElement: Element, fieldAnnotation: WCDBField, indexAnnotation: WCDBIndex?, defaultValue: WCDBDefault?): ColumnInfo {
            val resolvedInfo = ColumnInfo()
            resolvedInfo.propertyName = fieldElement.toString()
            resolvedInfo.propertyType = fieldElement.asType().toString()
            resolvedInfo.columnName = fieldAnnotation.columnName
            resolvedInfo.isPrimary = fieldAnnotation.isPrimary
            resolvedInfo.isAutoIncrement = fieldAnnotation.isAutoIncrement
            resolvedInfo.isUnique = fieldAnnotation.isUnique
            resolvedInfo.isNotNull = fieldAnnotation.isNotNull
            if(indexAnnotation != null) {
                resolvedInfo.indexNameSuffix = indexAnnotation.nameSuffix
                resolvedInfo.indexIsUnique = indexAnnotation.isUnique
            }
            if (defaultValue != null) {
                resolvedInfo.defaultValue = DefaultValueInfo.resolve(defaultValue)
            }
            return resolvedInfo
        }
    }
}