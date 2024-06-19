//
// Created by qiuwenchen on 2022/8/11.
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

#pragma once
#include "CPPDeclaration.h"
#include "MultiObject.hpp"
#include "Statement.hpp"
#include "Value.hpp"

namespace WCDB {

class WCDB_API StatementOperation {
public:
#pragma mark - Basic
    /**
     @brief Use `sqlite3_prepare` internally to prepare a new statement.
     @warning You should firstly use `StatementOperation::finalize()` to finalize the previous statement prepared by `StatementOperation::prepare()`. If you need to prepare multiple statements at the same time, `Handle::getOrCreatePreparedStatement()` is recommended.
     @see   `Handle::getOrCreatePreparedStatement()`
     @return True if no error occurs.
     */
    bool prepare(const Statement& statement);

    /**
     @brief Check if there is a statement previously prepared by `StatementOperation::prepare()`.
     @return True if there is a prepared statement.
     */
    bool isPrepared();

    /**
     @brief Finalize the statement previously prepared by `StatementOperation::prepare()`.
     @warning You should finalize the previously prepared statement before use a new statement.
     */
    void finalize();

    /**
     It is a wrapper for `sqlite3_step`.
     @warning Not Thread-safe.
     @return True if no error occurs.
     */
    bool step();

    /**
     @brief Check if you can continue stepping.
     @return True if you can continue stepping.
     */
    bool done();

    /**
     @brief The wrapper of `sqlite3_reset`.
     */
    void reset();

    /**
     @brief The wrapper of `sqlite3_clear_bindings`.
     */
    void clearBindings();

    /**
     @brief The wrapper of `sqlite3_stmt_readonly`.
     */
    bool isReadOnly();

#pragma mark - Bind Value
    using Integer = ColumnTypeInfo<ColumnType::Integer>::UnderlyingType;
    using Text = ColumnTypeInfo<ColumnType::Text>::UnderlyingType;
    using Float = ColumnTypeInfo<ColumnType::Float>::UnderlyingType;
    using BLOB = ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType;

    /**
     @brief The wrapper of `sqlite3_bind_int64`.
     */
    void bindInteger(const Integer& value, int index = 1);

    /**
     @brief The wrapper of `sqlite3_bind_double`.
     */
    void bindDouble(const Float& value, int index = 1);

    /**
     @brief The wrapper of `sqlite3_bind_text`.
     */
    void bindText(const Text& value, int index = 1);

    /**
     @brief The wrapper of `sqlite3_bind_blob`.
     */
    void bindBLOB(const BLOB& value, int index = 1);

    /**
     @brief The wrapper of `sqlite3_bind_null`.
     */
    void bindNull(int index = 1);

    /**
     @brief The wrapper of `sqlite3_bind_parameter_index`.
     */
    int bindParameterIndex(const Text& parameterName);

    /**
     @brief The wrapper of `sqlite3_bind_*`.
     It will call the appropriate routine according to the column type returned by `Value::getType()`.
     @param value A value wrapping the type.
     @see   `WCDB::Value`
     */
    void bindValue(const Value& value, int index = 1);

    /**
     @brief The wrapper of `sqlite3_bind_*` for binding an array of value.
     It will call the appropriate routine according to the column type returned by `Value::getType()`.
     @param row An array of value.
     @see   `WCDB::Value`
     */
    void bindRow(const OneRowValue& row);

    /**
     @brief The wrapper of `sqlite3_bind_*` for binding property of object to index.
     It will call the appropriate routine according to the data type of property.
     */
    template<class ObjectType>
    void bindObject(const ObjectType& obj, const Field& field, int index = 1)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        const BaseAccessor* accessor = field.getAccessor();
        switch (accessor->getColumnType()) {
        case ColumnType::Integer: {
            auto intAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::Integer>*>(accessor);
            if (!intAccessor->isNull(obj)) {
                bindInteger(intAccessor->getValue(obj), index);
            } else {
                bindNull(index);
            }
        } break;
        case ColumnType::Float: {
            auto floatAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::Float>*>(accessor);
            if (!floatAccessor->isNull(obj)) {
                bindDouble(floatAccessor->getValue(obj), index);
            } else {
                bindNull(index);
            }
        } break;
        case ColumnType::Text: {
            auto textAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::Text>*>(accessor);
            if (!textAccessor->isNull(obj)) {
                bindText(textAccessor->getValue(obj), index);
            } else {
                bindNull(index);
            }
        } break;
        case ColumnType::BLOB: {
            auto blobAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::BLOB>*>(accessor);
            if (!blobAccessor->isNull(obj)) {
                bindBLOB(blobAccessor->getValue(obj), index);
            } else {
                bindNull(index);
            }
        } break;
        default:
            assert(0);
            break;
        }
    }

    /**
     @brief The wrapper of `sqlite3_bind_*` for binding the specified properties of object.
     It will call the appropriate routine according to the data type of properties.
     */
    template<class ObjectType>
    void bindObject(const ObjectType& obj, const Fields& fields)
    {
        int index = 1;
        for (const Field& field : fields) {
            bindObject(obj, field, index);
            index++;
        }
    }

#pragma mark - Extract Column meta
    /**
     @brief The wrapper of `sqlite3_column_type`.
     */
    ColumnType getType(int index = 0);

    /**
     @brief The wrapper of `sqlite3_column_count`.
     */
    int getNumberOfColumns();

    /**
     @brief The wrapper of `sqlite3_column_origin_name`.
     */
    const UnsafeStringView getOriginColumnName(int index);

    /**
     @brief The wrapper of `sqlite3_column_name`.
     */
    const UnsafeStringView getColumnName(int index);

    /**
     @brief The wrapper of `sqlite3_column_table_name`.
     */
    const UnsafeStringView getColumnTableName(int index);

#pragma mark - Extract Row data
    /**
     @brief The wrapper of `sqlite3_column_int64`.
     */
    Integer getInteger(int index = 0);

    /**
     @brief The wrapper of `sqlite3_column_double`.
     */
    Float getDouble(int index = 0);

    /**
     @brief The wrapper of `sqlite3_column_text`.
     */
    Text getText(int index = 0);

    /**
     @brief The wrapper of `sqlite3_column_blob`.
     */
    const BLOB getBLOB(int index = 0);

    /**
     @brief The wrapper of `sqlite3_column_*`.
     */
    Value getValue(int index = 0);

    /**
     @brief Extract all values of current row.
     @return An array of value.
     */
    OneRowValue getOneRow();

    /**
     @brief Extract the values of the current row and assign them into the fields specified by resultFields of a new object.
     @return An object.
     */
    template<class ObjectType>
    ObjectType extractOneObject(const ResultFields& resultFields)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        ObjectType obj;
        int index = 0;
        for (const ResultField& field : resultFields) {
            const BaseAccessor* accessor = field.getAccessor();
            bool notNull = getType(index) != ColumnType::Null;
            switch (accessor->getColumnType()) {
            case ColumnType::Integer: {
                auto intAccessor
                = static_cast<const Accessor<ObjectType, ColumnType::Integer>*>(accessor);
                if (notNull) {
                    intAccessor->setValue(obj, getInteger(index));
                } else {
                    intAccessor->setNull(obj);
                }
            } break;
            case ColumnType::Float: {
                auto floatAccessor
                = static_cast<const Accessor<ObjectType, ColumnType::Float>*>(accessor);
                if (notNull) {
                    floatAccessor->setValue(obj, getDouble(index));
                } else {
                    floatAccessor->setNull(obj);
                }
            } break;
            case ColumnType::Text: {
                auto textAccessor
                = static_cast<const Accessor<ObjectType, ColumnType::Text>*>(accessor);
                if (notNull) {
                    textAccessor->setValue(obj, getText(index));
                } else {
                    textAccessor->setNull(obj);
                }
            } break;
            case ColumnType::BLOB: {
                auto blobAccessor
                = static_cast<const Accessor<ObjectType, ColumnType::BLOB>*>(accessor);
                if (notNull) {
                    blobAccessor->setValue(obj, getBLOB(index));
                } else {
                    blobAccessor->setNull(obj);
                }
            } break;
            default:
                assert(0);
                break;
            }
            index++;
        }
        return obj;
    }

    /**
     @brief Extract the results of a multi-table query in the current row.
     @return A `WCDB::MultiObject` containing the result of multi-table.
     @see   `WCDB::MultiObject`
     */
    MultiObject extractOneMultiObject(const ResultFields& resultFields);

#pragma mark - Step And Extract All Data
    /**
     @brief Extract all the values of the column of index in the result.
     @return An array of Value.
     */
    OptionalOneColumn getOneColumn(int index = 0);

    /**
     @brief Extract all the values in the result.
     @return A two-dimensional array.
     */
    OptionalMultiRows getAllRows();

    /**
     @brief Extract the values of all rows in the result and assign them into the fields specified by resultFields of new objects.
     @return An array of objects.
     */
    template<class ObjectType>
    OptionalValueArray<ObjectType> extractAllObjects(const ResultFields& resultFields)
    {
        OptionalValueArray<ObjectType> result;
        bool succeed = step();
        while (succeed && !done()) {
            if (!result.succeed()) {
                result = ValueArray<ObjectType>();
            }
            result->push_back(extractOneObject<ObjectType>(resultFields));
            succeed = step();
        }
        if (!succeed) {
            return NullOpt;
        }
        return !result.hasValue() ? ValueArray<ObjectType>() : result;
    }

    /**
     @brief Extract the results of a multi-table query.
     @return An array of `WCDB::MultiObject`.
     */
    OptionalMultiObjectArray extractAllMultiObjects(const ResultFields& resultFields);

protected:
    virtual ~StatementOperation() = 0;
    virtual HandleStatement* getInnerHandleStatement() = 0;
};

} //namespace WCDB
