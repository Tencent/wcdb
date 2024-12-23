// Created by qiuwenchen on 2023/6/25.
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

import com.tencent.wcdb.compiler.resolvedInfo.ColumnInfo
import com.tencent.wcdb.compiler.resolvedInfo.TableConfigInfo

private const val TAB = "\t"

class KotlinCodeGenerator {
    lateinit var packageName: String
    lateinit var className: String
    lateinit var ormClassName: String
    lateinit var tableConstraintInfo: TableConfigInfo
    lateinit var allColumnInfo: List<ColumnInfo>
    private lateinit var builder: StringBuilder

    fun generate(): String {
        builder = StringBuilder()
        builder.append("package $packageName\n\n")
        generateImport()
        builder.append("object $ormClassName: TableBinding<$className> {\n")

        builder.append("${TAB}private final val baseBinding: Binding\n")
        generateFields()
        builder.append("\n${TAB}init {\n")
        builder.append("$TAB${TAB}baseBinding = Binding()\n")
        generateColumns()
        generateTableConfig()
        builder.append("$TAB}\n\n")

        generateBindingType()
        generateBindingFields()
        generateBaseBinding()
        generateExtractObject()
        generateBindObject()
        generateAutoIncrementConfig()
        builder.append("}\n\n")
        return builder.toString()
    }

    private fun generateImport() {
        builder.append("import com.tencent.wcdb.core.PreparedStatement\n")
        builder.append("import com.tencent.wcdb.orm.*\n")
        builder.append("import com.tencent.wcdb.winq.Column\n")
        builder.append("import com.tencent.wcdb.winq.ColumnConstraint\n")
        builder.append("import com.tencent.wcdb.winq.ColumnDef\n")
        builder.append("import com.tencent.wcdb.winq.ColumnType\n")
        builder.append("import com.tencent.wcdb.winq.StatementCreateIndex\n")
        builder.append("import com.tencent.wcdb.winq.TableConstraint\n\n")
    }

    private fun generateFields() {
        for ((propertyName) in allColumnInfo) {
            builder.append("${TAB}@JvmField\n")
            builder.append("${TAB}final val $propertyName : Field<$className>\n")
        }
    }

    private fun generateColumns() {
        var fieldId = 1
        for (columnInfo in allColumnInfo) {
            val ormInfo = AllKotlinPropertyORMInfo[columnInfo.propertyType]!!
            val propertyName = columnInfo.propertyName
            var columnName = columnInfo.columnName
            if (columnName.isEmpty()) {
                columnName = propertyName
            }

            builder.append(
                "$TAB$TAB${propertyName} = Field(\"$columnName\", this, " +
                        "$fieldId, ${columnInfo.isPrimary && columnInfo.isAutoIncrement}, " +
                        "${columnInfo.isPrimary})\n"
            )
            fieldId++

            builder.append("$TAB${TAB}val ${propertyName}Def = ColumnDef($propertyName, ColumnType.${ormInfo.columnType})\n")

            val constraintPrefix =
                "$TAB$TAB${propertyName}Def.constraint(\n$TAB$TAB${TAB}ColumnConstraint()"
            if (columnInfo.isPrimary) {
                builder.append(constraintPrefix).append(".primaryKey()")
                    .append(if (columnInfo.isAutoIncrement) ".autoIncrement()\n$TAB$TAB)\n" else "\n$TAB$TAB)\n")
            }
            if (columnInfo.defaultValue != null) {
                if (ormInfo.columnType == "Integer") {
                    builder.append(constraintPrefix)
                        .append(".defaultTo(${columnInfo.defaultValue!!.intValue})\n$TAB$TAB)\n")
                } else if (ormInfo.columnType == "Float") {
                    builder.append(constraintPrefix)
                        .append(".defaultTo(${columnInfo.defaultValue!!.doubleValue})\n$TAB$TAB)\n")
                } else {
                    builder.append(constraintPrefix)
                        .append(".defaultTo(\"${columnInfo.defaultValue!!.textValue}\")\n$TAB$TAB)\n")
                }
            }
            if (columnInfo.isUnique) {
                builder.append(constraintPrefix).append(".unique()\n$TAB$TAB)\n")
            }
            if (columnInfo.isNotNull) {
                builder.append(constraintPrefix).append(".notNull()\n$TAB$TAB)\n")
            }
            if (columnInfo.isNotIndexed) {
                builder.append(constraintPrefix).append(".unIndex()\n$TAB$TAB)\n")
            }
            builder.append("$TAB${TAB}baseBinding.addColumnDef(${propertyName}Def)\n")

            if (columnInfo.enableAutoIncrementForExistingTable) {
                builder.append("$TAB${TAB}baseBinding.enableAutoIncrementForExistingTable()\n")
            }

            if (!columnInfo.hasIndex) {
                continue
            }
            var indexName = columnInfo.indexName
            var isFullName = true
            if (indexName.isEmpty()) {
                indexName = "_${columnName}_index"
                isFullName = false
            }
            builder.append("$TAB${TAB}baseBinding.addIndex(\"$indexName\" , $isFullName, StatementCreateIndex().ifNotExist()")
            if (columnInfo.indexIsUnique) {
                builder.append(".unique()")
            }
            builder.append(".indexedBy($propertyName))\n\n")
        }
    }

    private fun generateTableConfig() {
        val allColumns: MutableMap<String, ColumnInfo> = HashMap()
        for (columnInfo in allColumnInfo) {
            allColumns[columnInfo.columnName.ifEmpty { columnInfo.propertyName }] = columnInfo
        }
        for ((name, columns) in tableConstraintInfo.multiIndexes) {
            var indexName = name
            var isFullName = true
            if (indexName.isEmpty()) {
                indexName = columns.joinToString("_", "_", "_index")
                isFullName = false
            }
            builder.append("$TAB${TAB}baseBinding.addIndex(\"$indexName\", $isFullName, StatementCreateIndex().ifNotExist().indexedBy(\n$TAB$TAB$TAB")
            for (column in columns) {
                builder.append(allColumns[column]!!.propertyName).append(", ")
            }
            builder.append("\n$TAB$TAB))\n")
        }
        for ((columns) in tableConstraintInfo.multiPrimaries) {
            builder.append("$TAB${TAB}baseBinding.addTableConstraint(TableConstraint().primaryKey().indexedBy(\n$TAB$TAB$TAB")
            for (column in columns) {
                builder.append(allColumns[column]!!.propertyName).append(", ")
            }
            builder.append("\n$TAB$TAB))\n")
        }
        for ((columns) in tableConstraintInfo.multiUnique) {
            builder.append("$TAB${TAB}baseBinding.addTableConstraint(TableConstraint().unique().indexedBy(\n$TAB$TAB$TAB")
            for (column in columns) {
                builder.append(allColumns[column]!!.propertyName).append(", ")
            }
            builder.append("\n$TAB$TAB))\n")
        }
        if (tableConstraintInfo.isWithoutRowId) {
            builder.append("$TAB${TAB}baseBinding.configWithoutRowId()")
        }

        val ftsModuleInfo = tableConstraintInfo.ftsModule
        if (ftsModuleInfo == null || ftsModuleInfo.ftsVersion.isEmpty()) {
            return
        }

        builder.append("$TAB${TAB}baseBinding.configVirtualModule(\"${ftsModuleInfo.ftsVersion}\")\n")

        val tokenizer = java.lang.StringBuilder("tokenize = ")
        tokenizer.append(ftsModuleInfo.tokenizer)
        for (para in ftsModuleInfo.tokenizerParameters) {
            tokenizer.append(" ").append(para)
        }
        builder.append("$TAB${TAB}baseBinding.configVirtualModuleArgument(\"$tokenizer\")\n")
        if (ftsModuleInfo.externalTable.isNotEmpty()) {
            builder.append("$TAB${TAB}baseBinding.configVirtualModuleArgument(\"content='${ftsModuleInfo.externalTable}'\");\n")
        }
    }

    private fun generateBindingType() {
        builder.append("${TAB}override fun bindingType() = $className::class.java\n")
    }

    private fun generateBindingFields() {
        builder.append("${TAB}override fun allBindingFields() = allFields()\n")
        builder.append("${TAB}@JvmStatic\n")
        builder.append("${TAB}fun allFields() = arrayOf(")
        for ((propertyName) in allColumnInfo) {
            builder.append(propertyName).append(", ")
        }
        builder.append(")\n")
    }

    private fun generateBaseBinding() {
        builder.append("${TAB}override fun baseBinding() = baseBinding\n")
    }

    private fun generateExtractObject() {
        builder.append("${TAB}override fun <R : $className> extractObject(\n")
        builder.append("$TAB${TAB}fields: Array<Field<$className>>,\n")
        builder.append("$TAB${TAB}preparedStatement: PreparedStatement,\n")
        builder.append("$TAB${TAB}cls: Class<R>\n")
        builder.append("$TAB): R  {\n")
        builder.append("$TAB${TAB}val newOne = cls.newInstance()\n")
        builder.append("$TAB${TAB}var index = 0\n")
        builder.append("$TAB${TAB}for (field in fields) {\n")
        builder.append("$TAB$TAB${TAB}when (field.fieldId) {\n")
        var index = 1
        for ((propertyName, propertyType, nullable) in allColumnInfo) {
            val info = AllKotlinPropertyORMInfo[propertyType]!!
            if (nullable) {
                builder.append("$TAB$TAB$TAB$TAB${index} -> {\n")
                builder.append("$TAB$TAB$TAB$TAB${TAB}if ( preparedStatement.getColumnType(index) != ColumnType.Null) {\n")
                builder.append("$TAB$TAB$TAB$TAB$TAB${TAB}newOne.$propertyName = preparedStatement.${info.setter}\n")
                builder.append("$TAB$TAB$TAB$TAB${TAB}}\n")
                builder.append("$TAB$TAB$TAB${TAB}}\n")
            } else {
                builder.append("$TAB$TAB$TAB$TAB${index} -> newOne.$propertyName = preparedStatement.${info.setter}\n")
            }
            index++
        }
        builder.append("$TAB$TAB$TAB${TAB}else -> assert(false) {\n")
        builder.append("$TAB$TAB$TAB$TAB$TAB\"Invalid id \" + field.getFieldId() + \" of field \" + field.getDescription() + \" in $className.\"\n")
        builder.append("$TAB$TAB$TAB$TAB}\n")
        builder.append("$TAB$TAB$TAB}\n")
        builder.append("$TAB$TAB${TAB}index++\n")
        builder.append("$TAB$TAB}\n")
        builder.append("$TAB${TAB}return newOne\n")
        builder.append("$TAB}\n\n")
    }

    private fun generateBindObject() {
        builder.append("${TAB}override fun bindField(\n")
        builder.append("$TAB${TAB}`object`: $className,\n")
        builder.append("$TAB${TAB}field: Field<$className>,\n")
        builder.append("$TAB${TAB}index: Int,\n")
        builder.append("$TAB${TAB}preparedStatement: PreparedStatement\n")
        builder.append("$TAB) {\n")
        builder.append("$TAB${TAB}when (field.fieldId) {\n")
        var index = 1
        for ((propertyName, propertyType, nullable) in allColumnInfo) {
            val info = AllKotlinPropertyORMInfo[propertyType]!!
            if (nullable) {
                builder.append("$TAB$TAB$TAB${index} -> {\n")
                builder.append("$TAB$TAB$TAB${TAB}if (`object`.$propertyName != null) {\n")
                builder.append("$TAB$TAB$TAB$TAB${TAB}preparedStatement.${info.getter}(`object`.$propertyName, index)\n")
                builder.append("$TAB$TAB$TAB${TAB}} else {\n")
                builder.append("$TAB$TAB$TAB$TAB${TAB}preparedStatement.bindNull(index)\n")
                builder.append("$TAB$TAB$TAB${TAB}}\n")
                builder.append("$TAB$TAB${TAB}}\n")
            } else {
                builder.append("$TAB$TAB$TAB${index} -> preparedStatement.${info.getter}(`object`.$propertyName, index)\n")
            }
            index++
        }
        builder.append("$TAB$TAB${TAB}else -> assert(false) {\n")
        builder.append("$TAB$TAB$TAB$TAB\"Invalid id \" + field.getFieldId() + \" of field \" + field.getDescription() + \" in $className.\"\n")
        builder.append("$TAB$TAB$TAB}\n")
        builder.append("$TAB$TAB}\n")
        builder.append("$TAB}\n\n")
    }

    private fun generateAutoIncrementConfig() {
        builder.append("${TAB}override fun isAutoIncrement(`object`: $className): Boolean {\n")
        var autoIncrementColumn: ColumnInfo? = null
        for (info in allColumnInfo) {
            if (info.isAutoIncrement && info.isPrimary) {
                autoIncrementColumn = info
                break
            }
        }
        if (autoIncrementColumn != null) {
            builder.append("$TAB${TAB}return `object`.${autoIncrementColumn.propertyName} == 0\n")
        } else {
            builder.append("$TAB${TAB}return false\n")
        }
        builder.append("$TAB}\n\n")

        builder.append("${TAB}override fun setLastInsertRowId(`object`: $className, lastInsertRowId: Long) {\n")
        if (autoIncrementColumn != null) {
            var convert = ""
            when (autoIncrementColumn.propertyType) {
                Char::class.qualifiedName!! -> convert = ".toInt().toChar()"
                Byte::class.qualifiedName!! -> convert = ".toByte()"
                Short::class.qualifiedName!! -> convert = ".toShort()"
                Int::class.qualifiedName!! -> convert = ".toInt()"
            }
            builder.append("$TAB${TAB}`object`.${autoIncrementColumn.propertyName} = lastInsertRowId$convert\n")
        }
        builder.append("$TAB}\n\n")
    }
}