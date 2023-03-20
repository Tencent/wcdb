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
#import <WCDB/Value.hpp"

@class CPPTableTestCase;

#pragma mark - Create table

template<class ObjectType>
BOOL CPPTestTableCreate(CPPTableTestCase* testCase)
{
    return testCase.database->createTable<ObjectType>(testCase.tableName.UTF8String);
}

template<class ObjectType>
BOOL CPPTestVirtualTableCreate(CPPTableTestCase* testCase)
{
    return testCase.database->createVirtualTable<ObjectType>(testCase.tableName.UTF8String);
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
        row.push_back(field.getValue<ObjectType>(obj));
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
