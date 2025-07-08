//
// Created by qiuwenchen on 2022/8/27.
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

#include "Accessor.hpp"
#include "CPPDeclaration.h"
#include "MemberPointer.hpp"

namespace WCDB {

class WCDB_API ResultField final : public ResultColumn {
    friend class SyntaxList<ResultField>;
    friend class StatementOperation;

public:
    ResultField() = delete;

    ResultField(const Field& field);
    ResultField(const ResultColumn& resultColumn, const BaseAccessor* assessor);
    ~ResultField() override;

    template<class ORMType, typename FieldType>
    ResultField(FieldType ORMType::* memberPointer)
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
        bool notNull = !value.isNull();
        switch (m_accessor->getColumnType()) {
        case ColumnType::Integer: {
            auto intAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::Integer>*>(m_accessor);
            if (notNull) {
                intAccessor->setValue(obj, value.intValue());
            } else {
                intAccessor->setNull(obj);
            }
        } break;
        case ColumnType::Float: {
            auto floatAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::Float>*>(m_accessor);
            if (notNull) {
                floatAccessor->setValue(obj, value.floatValue());
            } else {
                floatAccessor->setNull(obj);
            }
        } break;
        case ColumnType::Text: {
            auto textAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::Text>*>(m_accessor);
            if (notNull) {
                textAccessor->setValue(obj, value.textValue());
            } else {
                textAccessor->setNull(obj);
            }
        } break;
        case ColumnType::BLOB: {
            auto blobAccessor
            = static_cast<const Accessor<ObjectType, ColumnType::BLOB>*>(m_accessor);
            if (notNull) {
                blobAccessor->setValue(obj, value.blobValue());
            } else {
                blobAccessor->setNull(obj);
            }
        } break;
        default:
            assert(0);
            break;
        }
    }

protected:
    const BaseAccessor* getAccessor() const;

private:
    void configWithBinding(const Binding& binding, void* memberPointer);
    const BaseAccessor* m_accessor;
};

template<>
class WCDB_API SyntaxList<ResultField> final : public _SyntaxList<ResultField> {
public:
    using _SyntaxList<ResultField>::_SyntaxList;
    ~SyntaxList() override;

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
