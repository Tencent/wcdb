//
// Created by 陈秋文 on 2022/8/27.
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

#include <WCDB/Accessor.hpp>
#include <WCDB/CPPDeclaration.h>
#include <WCDB/MemberPointer.hpp>

namespace WCDB {

class ResultField final : public ResultColumn {
    friend class SyntaxList<ResultField>;

public:
    ResultField() = delete;

    ResultField(const Field& field);
    ResultField(const ResultColumn& resultColumn, std::shared_ptr<BaseAccessor> assessor);
    ~ResultField() override final;

    template<class ORMType, typename FieldType>
    ResultField(FieldType ORMType::*memberPointer)
    {
        static_assert(ORMType::isObjectRelationMapped, "This class has no ORM configuration.");
        static_assert(IsFieldMemberPointer<FieldType ORMType::*>::value,
                      "Field should be initialize with a member field pointer, not a member function pointer");

        void* voidPointer = castMemberPointer(memberPointer);
        const Binding& binding = ORMType::getObjectRelationBinding();
        configWithBinding(binding, voidPointer);
    }

    template<class ObjectType>
    void setValue(ObjectType& obj, const Value& value) const
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        switch (m_accessor->getColumnType()) {
        case ColumnType::Integer: {
            auto intAccessor = dynamic_cast<Accessor<ObjectType, ColumnType::Integer>*>(
            m_accessor.get());
            intAccessor->setValue(obj, value.intValue());
        } break;
        case ColumnType::Float: {
            auto floatAccessor
            = dynamic_cast<Accessor<ObjectType, ColumnType::Float>*>(m_accessor.get());
            floatAccessor->setValue(obj, value.floatValue());
        } break;
        case ColumnType::Text: {
            auto textAccessor
            = dynamic_cast<Accessor<ObjectType, ColumnType::Text>*>(m_accessor.get());
            textAccessor->setValue(obj, value.textValue());
        } break;
        case ColumnType::BLOB: {
            auto blobAccessor
            = dynamic_cast<Accessor<ObjectType, ColumnType::BLOB>*>(m_accessor.get());
            blobAccessor->setValue(obj, value.blobValue());
        } break;
        case ColumnType::Null: {
            auto nullAccessor
            = dynamic_cast<Accessor<ObjectType, ColumnType::Null>*>(m_accessor.get());
            nullAccessor->setValue(obj, nullptr);
        } break;
        }
    }

protected:
    std::shared_ptr<BaseAccessor> getAccessor() const;

private:
    void configWithBinding(const Binding& binding, void* memberPointer);
    std::shared_ptr<BaseAccessor> m_accessor;
};

template<>
class SyntaxList<ResultField> final : public _SyntaxList<ResultField> {
public:
    using _SyntaxList<ResultField>::_SyntaxList;
    ~SyntaxList() override final;

    ResultFields
    resultColumnsByAddingNewResultColumns(const ResultFields& resultColumns) const;
    ResultFields& addingNewResultColumns(const ResultFields& resultColumns);

    ResultFields
    resultColumnsByRemovingResultColumns(const ResultFields& resultColumns) const;
    ResultFields& removingResultColumns(const ResultFields& resultColumns);

private:
    static bool isEqual(const ResultField& left, const ResultField& right);
};

} // namespace WCDB
