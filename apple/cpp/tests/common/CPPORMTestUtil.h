//
// Created by qiuwenchen on 2022/9/5.
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

#import "TestCaseAssertion.h"
#import <WCDB/CPPORM.h>
#import <WCDB/Value.hpp>

@class CPPTableTestCase;

#pragma mark - Create table

template<class ObjectType>
BOOL CPPTestTableCreate(CPPTableTestCase* testCase)
{
    return testCase.database->createTable<ObjectType>(testCase.tableName.UTF8String);
}

#pragma mark - create object

template<class ObjectType>
ObjectType CPPTestObjectCreate(const WCDB::OneRowValue& row)
{
    return CPPTestObjectCreate(row, ObjectType::allFields());
}

template<class ObjectType>
ObjectType CPPTestObjectCreate(const WCDB::OneRowValue& row, const WCDB::Fields& fields)
{
    ObjectType obj;
    TestCaseAssertTrue(row.size() == fields.size());
    int index = 0;
    for (const WCDB::Field& field : fields) {
        auto accessor = field.getAccessor();
        WCDB::Value value = row[index];
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer: {
            auto intAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::Integer>*>(accessor.get());
            intAccessor->setValue(obj, value);
        } break;
        case WCDB::ColumnType::Float: {
            auto floatAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::Float>*>(accessor.get());
            floatAccessor->setValue(obj, value);
        } break;
        case WCDB::ColumnType::Text: {
            auto textAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::Text>*>(accessor.get());
            textAccessor->setValue(obj, value);
        } break;
        case WCDB::ColumnType::BLOB: {
            auto blobAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::BLOB>*>(accessor.get());
            blobAccessor->setValue(obj, value);
        } break;
        case WCDB::ColumnType::Null: {
            auto nullAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::Null>*>(accessor.get());
            nullAccessor->setValue(obj, nullptr);
        } break;
        default:
            break;
        }
        index++;
    }
}

template<class ObjectType>
WCDB::ValueArray<ObjectType> CPPTestObjectsCreate(const WCDB::MultiRowsValue& rows, const WCDB::Fields& fields = WCDB::Fields())
{
    if (fields.size() == 0) {
        fields = ObjectType::allFields();
    }
    WCDB::ValueArray<ObjectType> result;
    for (const auto& row : rows) {
        result.push_back(CPPTestObjectCreate<ObjectType>(row, fields));
    }
    return result;
}

#pragma mark - extract value

template<class ObjectType>
WCDB::OneRowValue CPPOneRowValueExtract(const ObjectType& obj)
{
    return CPPOneRowValueExtract(obj, ObjectType::allFields());
}

template<class ObjectType>
WCDB::OneRowValue CPPOneRowValueExtract(const ObjectType& obj, const WCDB::Fields& fields)
{
    WCDB::OneRowValue row;
    for (const WCDB::Field& field : fields) {
        auto accessor = field.getAccessor();
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer: {
            auto intAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::Integer>*>(accessor.get());
            row.emplace_back(intAccessor->getValue(obj));
        } break;
        case WCDB::ColumnType::Float: {
            auto floatAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::Float>*>(accessor.get());
            row.emplace_back(floatAccessor->getValue(obj));
        } break;
        case WCDB::ColumnType::Text: {
            auto textAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::Text>*>(accessor.get());
            row.emplace_back(textAccessor->getValue(obj));
        } break;
        case WCDB::ColumnType::BLOB: {
            auto blobAccessor = dynamic_cast<WCDB::Accessor<ObjectType, WCDB::ColumnType::BLOB>*>(accessor.get());
            row.emplace_back(blobAccessor->getValue(obj));
        } break;
        case WCDB::ColumnType::Null: {
            row.emplace_back(nullptr);
        } break;
        }
    }
    return row;
}

template<class ObjectType>
WCDB::MultiRowsValue CPPMultiRowValueExtract(const WCDB::ValueArray<ObjectType>& objs)
{
    return CPPMultiRowValueExtract(objs, ObjectType::allFields());
}

template<class ObjectType>
WCDB::MultiRowsValue CPPMultiRowValueExtract(const WCDB::ValueArray<ObjectType>& objs, const WCDB::Fields& fields)
{
    WCDB::MultiRowsValue rows;
    for (const ObjectType& obj : objs) {
        rows.push_back(CPPOneRowValueExtract(obj, fields));
    }
    return rows;
}
