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

#import "CPPAllTypesObject.h"
#import "Random.h"

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPAllTypesObject)

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
WCDB_CPP_SYNTHESIZE(constCharArrValue)
WCDB_CPP_SYNTHESIZE(charArrValue)
WCDB_CPP_SYNTHESIZE(stdStringValue)
WCDB_CPP_SYNTHESIZE(unsafeStringViewValue)
WCDB_CPP_SYNTHESIZE(stringViewValue)

WCDB_CPP_SYNTHESIZE(blobValue)
WCDB_CPP_SYNTHESIZE(unsafeDataValue)
WCDB_CPP_SYNTHESIZE(dataValue)
WCDB_CPP_SYNTHESIZE(constUnsignedCharArrValue)
WCDB_CPP_SYNTHESIZE(unsignedCharArrValue)

WCDB_CPP_ORM_IMPLEMENTATION_END

CPPAllTypesObject CPPAllTypesObject::maxObject()
{
    CPPAllTypesObject object;
    object.type = "max";
#define ASSIGN_WITH_TYPED_MAX_VALUE(field, type) \
    object.field = std::numeric_limits<type>::max()
#define ASSIGN_WITH_MAX_VALUE(field) \
    ASSIGN_WITH_TYPED_MAX_VALUE(field, decltype(object.field))

    object.enumValue = EnumType::Max;
    object.enumClassValue = EnumClassType::Max;
    object.literalEnumValue = LiteralEnum::EnumMax;

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

    object.constCharpValue = "";
    object.charpValue = (char*) "";

    return object;
}

CPPAllTypesObject CPPAllTypesObject::minObject()
{
    CPPAllTypesObject object;
    object.type = "min";

#define ASSIGN_WITH_TYPED_MIN_VALUE(field, type) \
    object.field = std::numeric_limits<type>::min()
#define ASSIGN_WITH_MIN_VALUE(field) ASSIGN_WITH_TYPED_MIN_VALUE(field, decltype(object.field))

    object.enumValue = EnumType::Min;
    object.enumClassValue = EnumClassType::Min;
    object.literalEnumValue = LiteralEnum::EnumMin;

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

    object.constCharpValue = "";
    object.charpValue = (char*) "";

    return object;
}

CPPAllTypesObject CPPAllTypesObject::emptyObject()
{
    CPPAllTypesObject object;
    object.type = "nil";
    object.enumValue = EnumType::Zero;
    object.enumClassValue = EnumClassType::Zero;
    object.literalEnumValue = EnumZero;

    object.trueOrFalseValue = false;

    object.charValue = 0;
    object.unsignedCharValue = 0;
    object.shortValue = 0;
    object.unsignedShortValue = 0;
    object.intValue = 0;
    object.unsignedIntValue = 0;
    object.int32Value = 0;
    object.int64Value = 0;
    object.uint32Value = 0;
    object.uint64Value = 0;

    object.floatValue = 0;
    object.doubleValue = 0;

    object.constCharpValue = "";
    object.charpValue = (char*) "";
    return object;
}

CPPAllTypesObject CPPAllTypesObject::randomObject()
{
    CPPAllTypesObject object;
    object.type = "random";
    object.enumValue = EnumType::Zero;
    object.enumClassValue = EnumClassType::Zero;
    object.literalEnumValue = EnumZero;

    Random* random = [Random shared];

    object.trueOrFalseValue = random.boolean;

    object.charValue = random.int8;
    object.unsignedCharValue = random.uint8;
    object.shortValue = random.int16;
    object.unsignedShortValue = random.uint16;
    object.intValue = random.int32;
    object.unsignedIntValue = random.uint32;
    object.int32Value = random.int32;
    object.int64Value = random.int64;
    object.uint32Value = random.uint32;
    object.uint64Value = random.uint64;

    object.floatValue = random.float_;
    object.doubleValue = random.double_;

    object.stringViewValue = random.string.UTF8String;
    object.constCharpValue = object.stdStringValue.data();
    object.charpValue = const_cast<char*>(object.stdStringValue.data());
    object.stdStringValue = random.string.UTF8String;
    object.unsafeStringViewValue = object.stringViewValue;
    memcpy(object.charArrValue, [random stringWithLength:99].UTF8String, 100);

    NSData* data = random.data;
    const unsigned char* buffer = (const unsigned char*) data.bytes;
    object.blobValue.assign(buffer, buffer + data.length);
    data = random.data;
    buffer = (const unsigned char*) data.bytes;
    object.dataValue = WCDB::Data(buffer, data.length);
    object.unsafeDataValue = object.dataValue;
    memcpy(object.unsignedCharArrValue, [random dataWithLength:100].bytes, 100);

    return object;
}

bool CPPAllTypesObject::operator==(const CPPAllTypesObject& other)
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
                 && doubleValue == other.doubleValue;
    if (!equal) {
        return equal;
    }
    if (stringViewValue.compare(other.stringViewValue) != 0) {
        return false;
    }
    if (unsafeStringViewValue.compare(other.unsafeStringViewValue) != 0) {
        return false;
    }
    if ((constCharpValue == nullptr && other.constCharpValue != nullptr) || (constCharpValue != nullptr && other.constCharpValue == nullptr) || (constCharpValue != nullptr && other.constCharpValue != nullptr && strcmp(constCharpValue, other.constCharpValue) != 0)) {
        return false;
    }
    if ((charpValue == nullptr && other.charpValue != nullptr) || (charpValue != nullptr && other.charpValue == nullptr) || (charpValue != nullptr && other.charpValue != nullptr && strcmp(charpValue, other.charpValue) != 0)) {
        return false;
    }
    if (stdStringValue.compare(other.stdStringValue) != 0) {
        return false;
    }
    if (strcmp(charArrValue, other.charArrValue) != 0) {
        return false;
    }
    if (strcmp(constCharArrValue, other.constCharArrValue) != 0) {
        return false;
    }
    if (blobValue.size() != other.blobValue.size() || (blobValue.size() != 0 && memcmp(blobValue.data(), other.blobValue.data(), blobValue.size()) != 0)) {
        return false;
    }
    if (dataValue.size() != other.dataValue.size() || (dataValue.size() != 0 && memcmp(dataValue.buffer(), other.dataValue.buffer(), dataValue.size()) != 0)) {
        return false;
    }
    if (unsafeDataValue.size() != other.unsafeDataValue.size() || (unsafeDataValue.size() != 0 && memcmp(unsafeDataValue.buffer(), other.unsafeDataValue.buffer(), unsafeDataValue.size()) != 0)) {
        return false;
    }
    if (memcmp(constUnsignedCharArrValue, other.constUnsignedCharArrValue, 6) != 0) {
        return false;
    }
    if (memcmp(unsignedCharArrValue, other.unsignedCharArrValue, 100) != 0) {
        return false;
    }
    return true;
}

void CPPAllTypesObject::copyFrom(const CPPAllTypesObject& other)
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
    memcpy(charArrValue, other.charArrValue, 100);
    stdStringValue = other.stdStringValue;
    unsafeStringViewValue = other.unsafeStringViewValue;
    stringViewValue = other.stringViewValue;
    blobValue = other.blobValue;
    unsafeDataValue = other.unsafeDataValue;
    dataValue = other.dataValue;
    memcpy(unsignedCharArrValue, other.unsignedCharArrValue, 100);
}

CPPAllTypesObject& CPPAllTypesObject::operator=(const CPPAllTypesObject& other)
{
    copyFrom(other);
    return *this;
}

CPPAllTypesObject::CPPAllTypesObject(const CPPAllTypesObject& other)
{
    copyFrom(other);
}

CPPAllTypesObject::CPPAllTypesObject(CPPAllTypesObject&& other)
{
    copyFrom(other);
}

CPPAllTypesObject::CPPAllTypesObject() = default;
CPPAllTypesObject::~CPPAllTypesObject() = default;
