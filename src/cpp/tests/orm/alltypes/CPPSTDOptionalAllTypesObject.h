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

#include "CPPAllTypesObject.h"

#if defined(__cplusplus) && __cplusplus > 201402L

class CPPSTDOptionalAllTypesObject {
public:
    WCDB::StringView type;

    // Enum
    std::optional<EnumType> enumValue;
    std::optional<EnumClassType> enumClassValue;
    std::optional<LiteralEnum> literalEnumValue;

    // Bool
    std::optional<bool> trueOrFalseValue;

    // Integer
    std::optional<char> charValue;
    std::optional<unsigned char> unsignedCharValue;
    std::optional<short> shortValue;
    std::optional<unsigned short> unsignedShortValue;
    std::optional<int> intValue;
    std::optional<unsigned int> unsignedIntValue;
    std::optional<int32_t> int32Value;
    std::optional<int64_t> int64Value;
    std::optional<uint32_t> uint32Value;
    std::optional<uint64_t> uint64Value;

    // Float
    std::optional<float> floatValue;
    std::optional<double> doubleValue;

    // String
    std::optional<const char*> constCharpValue;
    std::optional<char*> charpValue;
    std::optional<std::string> stdStringValue;
    std::optional<WCDB::UnsafeStringView> unsafeStringViewValue;
    std::optional<WCDB::StringView> stringViewValue;

    // BLOB
    std::optional<std::vector<unsigned char>> blobValue;
    std::optional<WCDB::UnsafeData> unsafeDataValue;
    std::optional<WCDB::Data> dataValue;

    WCDB_CPP_ORM_DECLARATION(CPPSTDOptionalAllTypesObject)

private:
    void copyFrom(const CPPSTDOptionalAllTypesObject& other);

public:
    static CPPSTDOptionalAllTypesObject maxObject();
    static CPPSTDOptionalAllTypesObject minObject();
    static CPPSTDOptionalAllTypesObject emptyObject();
    static CPPSTDOptionalAllTypesObject randomObject();

    bool operator==(const CPPSTDOptionalAllTypesObject& other);
    CPPSTDOptionalAllTypesObject& operator=(const CPPSTDOptionalAllTypesObject& other);
    CPPSTDOptionalAllTypesObject(const CPPSTDOptionalAllTypesObject& other);
    CPPSTDOptionalAllTypesObject(CPPSTDOptionalAllTypesObject&& other);
    CPPSTDOptionalAllTypesObject();
    ~CPPSTDOptionalAllTypesObject();
};

#endif
