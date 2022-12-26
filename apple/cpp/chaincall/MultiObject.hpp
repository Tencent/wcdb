//
// Created by 陈秋文 on 2022/10/24.
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

#include <WCDB/CPPORM.h>

namespace WCDB {

class MultiObject {
    friend class StatementOperation;

public:
    MultiObject();
    ~MultiObject();

    /**
     @brief Get result of specified table.
     @param table Table name.
     @return an object of specified type.
     */
    template<class ObjectType>
    std::optional<ObjectType> objectAtTable(const UnsafeStringView& table)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        auto iter = m_objs.find(table);
        if (iter == m_objs.end()) {
            return std::optional<ObjectType>();
        }
        ObjectType result;
        ObjectValue& objValue = iter->second;
        for (ResultField& field : objValue.m_fields) {
            const StringView& fieldName = field.syntax().expression.column.name;
            field.setValue(result, objValue.m_values[fieldName]);
        }
        return result;
    }

    /**
     @brief add an object as result of the specified table.
     @param obj The object to add.
     @param table Table name.
     */
    template<class ObjectType>
    void addObject(const ObjectType& obj, const UnsafeStringView& table)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        const Fields fields = ObjectType::allFields();
        for (const Field& field : fields) {
            addField(table, field, field.getValue(obj));
        }
    }

    bool operator==(const MultiObject& other) const;
    bool operator!=(const MultiObject& other) const;

protected:
    void addField(const UnsafeStringView& table, const ResultField& field, const Value& value);

private:
    typedef struct ObjectValue {
        ResultFields m_fields;
        StringViewMap<Value> m_values;
        void addField(const ResultField& field, const Value& value);
        bool operator==(const ObjectValue& other) const;
        bool operator!=(const ObjectValue& other) const;
    } ObjectValue;
    StringViewMap<ObjectValue> m_objs;
};

} // namespace WCDB
