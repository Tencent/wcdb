//
// Created by qiuwenchen on 2023/11/1.
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

#import "CPPSTDOptionalAllTypesObject.h"
#import <Foundation/Foundation.h>

#import "Random.h"

#if defined(__cplusplus) && __cplusplus > 201402L

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPSTDOptionalAllTypesObject)

WCDB_CPP_SYNTHESIZE(type)

WCDB_CPP_SYNTHESIZE(enumValue)
WCDB_CPP_SYNTHESIZE(enumClassValue)
WCDB_CPP_SYNTHESIZE(literalEnumValue)

WCDB_CPP_SYNTHESIZE(trueOrFalseValue)

WCDB_CPP_SYNTHESIZE(charValue)
WCDB_CPP_SYNTHESIZE(unsignedCharValue)
WCDB_CPP_SYNTHESIZE(shortValue)
WCDB_CPP_SYNTHESIZE(unsignedShortValue)
WCDB_CPP_SYNTHESIZE(intValue)
WCDB_CPP_SYNTHESIZE(unsignedIntValue)
WCDB_CPP_SYNTHESIZE(int32Value)
WCDB_CPP_SYNTHESIZE(int64Value)
WCDB_CPP_SYNTHESIZE(uint32Value)
WCDB_CPP_SYNTHESIZE(uint64Value)

WCDB_CPP_SYNTHESIZE(floatValue)
WCDB_CPP_SYNTHESIZE(doubleValue)

WCDB_CPP_SYNTHESIZE(constCharpValue)
WCDB_CPP_SYNTHESIZE(charpValue)
WCDB_CPP_SYNTHESIZE(stdStringValue)
WCDB_CPP_SYNTHESIZE(unsafeStringViewValue)
WCDB_CPP_SYNTHESIZE(stringViewValue)

WCDB_CPP_SYNTHESIZE(blobValue)
WCDB_CPP_SYNTHESIZE(unsafeDataValue)
WCDB_CPP_SYNTHESIZE(dataValue)

WCDB_CPP_ORM_IMPLEMENTATION_END

CPPSTDOptionalAllTypesObject CPPSTDOptionalAllTypesObject::maxObject()
{
    CPPSTDOptionalAllTypesObject object;
    object.type = "max";

    object.enumValue = std::make_optional<EnumType>(EnumType::Max);
    object.enumClassValue = std::make_optional<EnumClassType>(EnumClassType::Max);
    object.literalEnumValue = std::make_optional<LiteralEnum>(LiteralEnum::EnumMax);

#define ASSIGN_WITH_TYPED_MAX_VALUE(field, type) \
    object.field = std::make_optional<typename type::value_type>(std::numeric_limits<typename type::value_type>::max())
#define ASSIGN_WITH_MAX_VALUE(field) \
    ASSIGN_WITH_TYPED_MAX_VALUE(field, decltype(object.field))

    ASSIGN_WITH_MAX_VALUE(trueOrFalseValue);

    ASSIGN_WITH_MAX_VALUE(charValue);
    ASSIGN_WITH_MAX_VALUE(unsignedCharValue);
    ASSIGN_WITH_MAX_VALUE(shortValue);
    ASSIGN_WITH_MAX_VALUE(unsignedShortValue);
    ASSIGN_WITH_MAX_VALUE(intValue);
    ASSIGN_WITH_MAX_VALUE(unsignedIntValue);
    ASSIGN_WITH_MAX_VALUE(int32Value);
    ASSIGN_WITH_MAX_VALUE(int64Value);
    ASSIGN_WITH_MAX_VALUE(uint32Value);
    ASSIGN_WITH_MAX_VALUE(uint64Value);

    ASSIGN_WITH_MAX_VALUE(floatValue);
    ASSIGN_WITH_MAX_VALUE(doubleValue);

    return object;
}

CPPSTDOptionalAllTypesObject CPPSTDOptionalAllTypesObject::minObject()
{
    CPPSTDOptionalAllTypesObject object;
    object.type = "min";

    object.enumValue = std::make_optional<EnumType>(EnumType::Min);
    object.enumClassValue = std::make_optional<EnumClassType>(EnumClassType::Min);
    object.literalEnumValue = std::make_optional<LiteralEnum>(LiteralEnum::EnumMin);

#define ASSIGN_WITH_TYPED_MIN_VALUE(field, type) \
    object.field = std::make_optional<typename type::value_type>(std::numeric_limits<typename type::value_type>::min())
#define ASSIGN_WITH_MIN_VALUE(field) ASSIGN_WITH_TYPED_MIN_VALUE(field, decltype(object.field))

    ASSIGN_WITH_MIN_VALUE(trueOrFalseValue);

    ASSIGN_WITH_MIN_VALUE(charValue);
    ASSIGN_WITH_MIN_VALUE(unsignedCharValue);
    ASSIGN_WITH_MIN_VALUE(shortValue);
    ASSIGN_WITH_MIN_VALUE(unsignedShortValue);
    ASSIGN_WITH_MIN_VALUE(intValue);
    ASSIGN_WITH_MIN_VALUE(unsignedIntValue);
    ASSIGN_WITH_MIN_VALUE(int32Value);
    ASSIGN_WITH_MIN_VALUE(int64Value);
    ASSIGN_WITH_MIN_VALUE(uint32Value);
    ASSIGN_WITH_MIN_VALUE(uint64Value);

    ASSIGN_WITH_MIN_VALUE(floatValue);
    ASSIGN_WITH_MIN_VALUE(doubleValue);

    return object;
}

CPPSTDOptionalAllTypesObject CPPSTDOptionalAllTypesObject::emptyObject()
{
    CPPSTDOptionalAllTypesObject object;
    object.type = "empty";
    return object;
}

CPPSTDOptionalAllTypesObject CPPSTDOptionalAllTypesObject::randomObject()
{
    CPPSTDOptionalAllTypesObject object;
    object.type = "random";

    object.enumValue = std::make_optional<EnumType>(EnumType::Zero);
    object.enumClassValue = std::make_optional<EnumClassType>(EnumClassType::Zero);
    object.literalEnumValue = std::make_optional<LiteralEnum>(LiteralEnum::EnumZero);

    Random* random = [Random shared];

#define OPTIONAL_RANDOM_VALUE(value) std::make_optional<decltype(value)>(value)

    object.trueOrFalseValue = OPTIONAL_RANDOM_VALUE((bool) random.boolean);

    object.charValue = OPTIONAL_RANDOM_VALUE((char) random.int8);
    object.unsignedCharValue = OPTIONAL_RANDOM_VALUE(random.uint8);
    object.shortValue = OPTIONAL_RANDOM_VALUE(random.int16);
    object.unsignedShortValue = OPTIONAL_RANDOM_VALUE(random.uint16);
    object.intValue = OPTIONAL_RANDOM_VALUE(random.int32);
    object.unsignedIntValue = OPTIONAL_RANDOM_VALUE(random.uint32);
    object.int32Value = OPTIONAL_RANDOM_VALUE(random.int32);
    object.int64Value = OPTIONAL_RANDOM_VALUE(random.int64);
    object.uint32Value = OPTIONAL_RANDOM_VALUE(random.uint32);
    object.uint64Value = OPTIONAL_RANDOM_VALUE(random.uint64);

    object.floatValue = OPTIONAL_RANDOM_VALUE(random.float_);
    object.doubleValue = OPTIONAL_RANDOM_VALUE(random.double_);

    object.stringViewValue = OPTIONAL_RANDOM_VALUE((WCDB::StringView) random.string.UTF8String);
    object.stdStringValue = OPTIONAL_RANDOM_VALUE((std::string) random.string.UTF8String);
    object.constCharpValue = OPTIONAL_RANDOM_VALUE(object.stdStringValue->data());
    object.charpValue = OPTIONAL_RANDOM_VALUE(const_cast<char*>(object.stdStringValue->data()));
    object.unsafeStringViewValue = object.stringViewValue;

    NSData* data = random.data;
    const unsigned char* buffer = (const unsigned char*) data.bytes;
    object.blobValue = std::make_optional<std::vector<unsigned char>>();
    object.blobValue->assign(buffer, buffer + data.length);
    data = random.data;
    buffer = (const unsigned char*) data.bytes;
    object.dataValue = OPTIONAL_RANDOM_VALUE(WCDB::Data(buffer, data.length));
    object.unsafeDataValue = object.dataValue;

    return object;
}

bool CPPSTDOptionalAllTypesObject::operator==(const CPPSTDOptionalAllTypesObject& other)
{
    bool equal = enumValue == other.enumValue
                 && enumClassValue == other.enumClassValue
                 && literalEnumValue == other.literalEnumValue
                 && trueOrFalseValue == other.trueOrFalseValue
                 && charValue == other.charValue
                 && unsignedCharValue == other.unsignedCharValue
                 && shortValue == other.shortValue
                 && unsignedShortValue == other.unsignedShortValue
                 && intValue == other.intValue
                 && unsignedIntValue == other.unsignedIntValue
                 && int32Value == other.int32Value
                 && int64Value == other.int64Value
                 && uint32Value == other.uint32Value
                 && uint64Value == other.uint64Value
                 && floatValue == other.floatValue
                 && doubleValue == other.doubleValue
                 && stringViewValue == other.stringViewValue
                 && unsafeStringViewValue == other.unsafeStringViewValue
                 && unsafeStringViewValue == other.unsafeStringViewValue
                 && stdStringValue == other.stdStringValue
                 && blobValue == other.blobValue
                 && dataValue == other.dataValue
                 && unsafeDataValue == other.unsafeDataValue;
    if (!equal) {
        return equal;
    }
#define CheckNullEquelOrReturn(value)                                                                                       \
    if ((value != std::nullopt && other.value == std::nullopt) || (value == std::nullopt && other.value != std::nullopt)) { \
        return false;                                                                                                       \
    }
    CheckNullEquelOrReturn(constCharpValue);
    if (constCharpValue != other.constCharpValue && strcmp(*constCharpValue, *other.constCharpValue) != 0) {
        return false;
    }
    CheckNullEquelOrReturn(charpValue);
    if (charpValue != other.charpValue && strcmp(*charpValue, *other.charpValue) != 0) {
        return false;
    }
    return true;
}

void CPPSTDOptionalAllTypesObject::copyFrom(const CPPSTDOptionalAllTypesObject& other)
{
    type = other.type;
    enumValue = other.enumValue;
    enumClassValue = other.enumClassValue;
    literalEnumValue = other.literalEnumValue;
    trueOrFalseValue = other.trueOrFalseValue;
    charValue = other.charValue;
    unsignedCharValue = other.unsignedCharValue;
    shortValue = other.shortValue;
    unsignedShortValue = other.unsignedShortValue;
    intValue = other.intValue;
    unsignedIntValue = other.unsignedIntValue;
    int32Value = other.int32Value;
    int64Value = other.int64Value;
    uint32Value = other.uint32Value;
    uint64Value = other.uint64Value;
    floatValue = other.floatValue;
    doubleValue = other.doubleValue;
    constCharpValue = other.constCharpValue;
    charpValue = other.charpValue;
    stdStringValue = other.stdStringValue;
    unsafeStringViewValue = other.unsafeStringViewValue;
    stringViewValue = other.stringViewValue;
    blobValue = other.blobValue;
    unsafeDataValue = other.unsafeDataValue;
    dataValue = other.dataValue;
}

CPPSTDOptionalAllTypesObject& CPPSTDOptionalAllTypesObject::operator=(const CPPSTDOptionalAllTypesObject& other)
{
    copyFrom(other);
    return *this;
}

CPPSTDOptionalAllTypesObject::CPPSTDOptionalAllTypesObject(const CPPSTDOptionalAllTypesObject& other)
{
    copyFrom(other);
}

CPPSTDOptionalAllTypesObject::CPPSTDOptionalAllTypesObject(CPPSTDOptionalAllTypesObject&& other)
{
    copyFrom(other);
}

CPPSTDOptionalAllTypesObject::CPPSTDOptionalAllTypesObject() = default;
CPPSTDOptionalAllTypesObject::~CPPSTDOptionalAllTypesObject() = default;

#endif
