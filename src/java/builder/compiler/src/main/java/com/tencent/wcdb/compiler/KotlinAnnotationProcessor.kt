// Created by qiuwenchen on 2023/6/20.
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

package com.tencent.wcdb.compiler

import com.google.devtools.ksp.processing.*
import com.google.devtools.ksp.symbol.*
import com.google.devtools.ksp.validate
import com.tencent.wcdb.WCDBDefault
import com.tencent.wcdb.WCDBField
import com.tencent.wcdb.WCDBIndex
import com.tencent.wcdb.WCDBTableCoding
import com.tencent.wcdb.compiler.resolvedInfo.ColumnInfo
import com.tencent.wcdb.compiler.resolvedInfo.TableConstraintInfo
import kotlin.reflect.KClass

const val OPTION_VERBOSE = "verbose"

internal class KotlinAnnotationProcessor(private val environment: SymbolProcessorEnvironment): KSVisitorVoid(), SymbolProcessor {
    class Provider : SymbolProcessorProvider {
        override fun create(environment: SymbolProcessorEnvironment): SymbolProcessor {
            return KotlinAnnotationProcessor(environment)
        }
    }

    private var verbose = false
    private var isKotlin = false
    private var primaryKeyCount = 0;
    private var tableConstraintInfo: TableConstraintInfo? = null
    private val allPropertyInfo = mutableListOf<ColumnInfo>()

    override fun process(resolver: Resolver): List<KSAnnotated> {
        environment.logger.warn("WCDB KSP Processing")
        verbose = environment.options.contains(OPTION_VERBOSE)
        val symbols = resolver.getSymbolsWithAnnotation(WCDBTableCoding::class.qualifiedName!!)
        val ret = mutableListOf<KSAnnotated>()
        symbols.toList().forEach {
            if (!it.validate())
                ret.add(it)
            else
                it.accept(this, Unit)
        }
        return ret
    }

    override fun visitClassDeclaration(classDeclaration: KSClassDeclaration, data: Unit) {

        verboseLog("WCDB Processing: ${classDeclaration.qualifiedName!!.asString()}")

        if (!checkClass(classDeclaration)) {
            return
        }

        tableConstraintInfo = TableConstraintInfo.resolve(
            getAnnotation(classDeclaration, WCDBTableCoding::class),
            environment.logger
        )
        if (tableConstraintInfo == null) {
            environment.logger.error("Failed to resolve @WCDBTableCoding in ${classDeclaration.qualifiedName!!.asString()}!")
            return
        }
        isKotlin = classDeclaration.containingFile!!.fileName.endsWith(".kt")
        primaryKeyCount = 0;
        allPropertyInfo.clear()

        for (property in classDeclaration.getAllProperties()) {

            val fieldAnnotation = getAnnotation(property, WCDBField::class) ?: continue
            val indexAnnotation = getAnnotation(property, WCDBIndex::class)
            val defaultValueAnnotation = getAnnotation(property, WCDBDefault::class)

            val resolvedColumnInfo =
                ColumnInfo.resolve(
                    property,
                    fieldAnnotation,
                    indexAnnotation,
                    defaultValueAnnotation,
                    environment.logger)
                    ?: return

            if (!checkProperty(property, resolvedColumnInfo)) return

            verboseLog("Found WCDBField: ${property.simpleName.asString()}")
            allPropertyInfo.add(resolvedColumnInfo)
        }

        if (!checkColumnInTableConstraint(classDeclaration)) {
            return
        }

        if (allPropertyInfo.isNotEmpty()) {
            createORMFile(classDeclaration)
        }
    }

    private fun verboseLog(content: String) {
        if (verbose) {
            environment.logger.info(content)
        }
    }

    private fun checkClass(classDeclaration: KSClassDeclaration): Boolean {
        if (classDeclaration.classKind != ClassKind.CLASS) {
            environment.logger.error(
                "@WCDBTableCoding can only be apply to normal class",
                classDeclaration
            )
            return false
        }
        val modifiers = classDeclaration.modifiers
        if (modifiers.contains(Modifier.PRIVATE) || modifiers.contains(Modifier.PROTECTED)) {
            environment.logger.error(
                "The class with annotation @WCDBTableCoding can not be private or protected",
                classDeclaration
            )
            return false
        }
        if (modifiers.contains(Modifier.ABSTRACT)) {
            environment.logger.error(
                "The class with annotation @WCDBTableCoding can not be abstract",
                classDeclaration
            )
            return false
        }
        return true
    }

    private fun <T : Any> getAnnotation(
        declaration: KSDeclaration,
        annotationClass: KClass<T>
    ): KSAnnotation? {
        return declaration.annotations.find {
            it.shortName.asString() == annotationClass.simpleName!! &&
                    it.annotationType.resolve().declaration.qualifiedName!!.asString() == annotationClass.qualifiedName!!
        }
    }

    private fun checkProperty(
        declaration: KSPropertyDeclaration,
        columnInfo: ColumnInfo
    ): Boolean {
        val modifier = declaration.modifiers
        if (modifier.contains(Modifier.PRIVATE) || modifier.contains(Modifier.PROTECTED)) {
            environment.logger.error(
                "The field with annotation @WCDBField can not be private or protected",
                declaration
            )
            return false
        }
        if (!AllKotlinPropertyTypes.contains(columnInfo.propertyType)) {
            var message = """
                    The type ${columnInfo.propertyType} of field ${declaration.simpleName.asString()} in 
                    ${declaration.parentDeclaration!!.qualifiedName!!.asString()} is Unsupported!
                    """
            message += if (!isKotlin) {
                "WCDB only supports Java basic types and byte[]."
            } else {
                "WCDB only supports Kotlin basic types and ByteArray."
            }
            environment.logger.error(message, declaration)
            return false
        }
        if (columnInfo.isPrimary) {
            primaryKeyCount++
            if (primaryKeyCount > 1) {
                val message = """@WCDBField can only configure one primary key in 
                    ${declaration.parentDeclaration!!.qualifiedName!!.asString()}. 
                    If multiple primary keys are required, configure multiPrimaries in @WCDBTableCoding. 
                    """
                environment.logger.error(message, declaration)
                return false
            }
            if (columnInfo.isAutoIncrement) {
                if (AllKotlinPropertyORMInfo[columnInfo.propertyType]?.columnType != "Integer") {
                    environment.logger.error("Auto-increment field must be integer", declaration)
                    return false
                }
            }
            if (columnInfo.hasIndex) {
                environment.logger.error("${declaration.simpleName.asString()} is confined as primary key, so it does not need to configure @WCDBIndex", declaration)
                return false
            }
        } else if (columnInfo.isAutoIncrement) {
            environment.logger.error(
                "Only the primary key can be configured as auto-increment",
                declaration
            )
            return false
        }
        if (columnInfo.defaultValue != null) {
            var valueCount = 0
            val columnType = AllKotlinPropertyORMInfo[columnInfo.propertyType]!!.columnType
            var typeMissMatch = false
            if (columnInfo.defaultValue!!.intValue != 0L) {
                valueCount++
                if (columnType != "Integer") {
                    typeMissMatch = true
                }
            }
            if (columnInfo.defaultValue!!.doubleValue != 0.0) {
                valueCount++
                if (columnType != "Float") {
                    typeMissMatch = true
                }
            }
            if (columnInfo.defaultValue!!.textValue.isNotEmpty()) {
                valueCount++
                if (columnType != "Text") {
                    typeMissMatch = true
                }
            }
            if (valueCount > 1) {
                environment.logger.error(
                    "Only one default value can be configured for a field",
                    declaration
                )
                return false
            } else if (typeMissMatch) {
                if (columnType == "BLOB'") {
                    environment.logger.error(
                        "Assigning a default value to BLOB is unsupported",
                        declaration
                    )
                } else {
                    environment.logger.error(
                        "Default value should be a $columnType",
                        declaration
                    )
                }
                return false
            }
        }
        return true
    }

    private fun checkColumnInTableConstraint(classDeclaration: KSClassDeclaration): Boolean {
        if (tableConstraintInfo!!.multiIndexes.isEmpty() &&
                tableConstraintInfo!!.multiPrimaries.isEmpty() &&
                tableConstraintInfo!!.multiUnique.isEmpty()) {
            return true;
        }

        val allColumns = allPropertyInfo.map { columnInfo ->
            if (columnInfo.columnName.isEmpty()) columnInfo.propertyName else columnInfo.columnName
        }.toSet()

        for(multiIndexes in tableConstraintInfo!!.multiIndexes) {
            for(column in multiIndexes.columns) {
                if (!allColumns.contains(column)) {
                    environment.logger.error(
                        "Can't find column \"$column\" in class orm config.",
                        classDeclaration
                    )
                    return false
                }
            }
        }

        for(multiPrimaries in tableConstraintInfo!!.multiPrimaries) {
            for(column in multiPrimaries.columns) {
                if (!allColumns.contains(column)) {
                    environment.logger.error(
                        "Can't find column \"$column\" in class orm config.",
                        classDeclaration
                    )
                    return false
                }
            }
        }

        for(multiUnique in tableConstraintInfo!!.multiUnique) {
            for(column in multiUnique.columns) {
                if (!allColumns.contains(column)) {
                    environment.logger.error(
                        "Can't find column \"$column\" in class orm config.",
                        classDeclaration
                    )
                    return false
                }
            }
        }
        return true
    }

    private fun createORMFile(classDeclaration: KSClassDeclaration) {
        val className = classDeclaration.simpleName.asString()
        val ormClassName = "DB${className}"
        val packageName = classDeclaration.containingFile!!.packageName.asString()
        val code: String

        if (isKotlin) {
            val codeGenerator = KotlinCodeGenerator()
            codeGenerator.packageName = packageName
            codeGenerator.className = className
            codeGenerator.ormClassName = ormClassName
            codeGenerator.tableConstraintInfo = tableConstraintInfo!!
            codeGenerator.allColumnInfo = allPropertyInfo
            code = codeGenerator.generate()
        } else {
            for(propertyInfo in allPropertyInfo) {
                propertyInfo.propertyType = K2JTypeMap[propertyInfo.propertyType]!!
            }
            val codeGenerator = JavaCodeGenerator()
            codeGenerator.packageName = packageName
            codeGenerator.className = className
            codeGenerator.ormClassName = ormClassName
            codeGenerator.tableConstraintInfo = tableConstraintInfo
            codeGenerator.allColumnInfo = allPropertyInfo
            code = codeGenerator.generate()
        }

        val file = environment.codeGenerator.createNewFile(
            Dependencies(
                false,
                classDeclaration.containingFile!!
            ),
            packageName,
            ormClassName,
            if(isKotlin) "kt" else "java"
        )
        file.write(code.toByteArray())
        file.close()
    }
}