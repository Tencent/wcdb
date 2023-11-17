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

class CPPWCDBOptionalAllTypesObject {
public:
    WCDB::StringView type;

    // Enum
    WCDB::Optional<EnumType> enumValue;
    WCDB::Optional<EnumClassType> enumClassValue;
    WCDB::Optional<LiteralEnum> literalEnumValue;

    // Bool
    WCDB::Optional<bool> trueOrFalseValue;

    // Integer
    WCDB::Optional<char> charValue;
    WCDB::Optional<unsigned char> unsignedCharValue;
    WCDB::Optional<short> shortValue;
    WCDB::Optional<unsigned short> unsignedShortValue;
    WCDB::Optional<int> intValue;
    WCDB::Optional<unsigned int> unsignedIntValue;
    WCDB::Optional<int32_t> int32Value;
    WCDB::Optional<int64_t> int64Value;
    WCDB::Optional<uint32_t> uint32Value;
    WCDB::Optional<uint64_t> uint64Value;

    // Float
    WCDB::Optional<float> floatValue;
    WCDB::Optional<double> doubleValue;

    // String
    WCDB::Optional<const char*> constCharpValue;
    WCDB::Optional<char*> charpValue;
    WCDB::Optional<std::string> stdStringValue;
    WCDB::Optional<WCDB::UnsafeStringView> unsafeStringViewValue;
    WCDB::Optional<WCDB::StringView> stringViewValue;

    // BLOB
    WCDB::Optional<std::vector<unsigned char>> blobValue;
    WCDB::Optional<WCDB::UnsafeData> unsafeDataValue;
    WCDB::Optional<WCDB::Data> dataValue;

    WCDB_CPP_ORM_DECLARATION(CPPWCDBOptionalAllTypesObject)

private:
    void copyFrom(const CPPWCDBOptionalAllTypesObject& other);

public:
    static CPPWCDBOptionalAllTypesObject maxObject();
    static CPPWCDBOptionalAllTypesObject minObject();
    static CPPWCDBOptionalAllTypesObject emptyObject();
    static CPPWCDBOptionalAllTypesObject randomObject();

    bool operator==(const CPPWCDBOptionalAllTypesObject& other);
    CPPWCDBOptionalAllTypesObject& operator=(const CPPWCDBOptionalAllTypesObject& other);
    CPPWCDBOptionalAllTypesObject(const CPPWCDBOptionalAllTypesObject& other);
    CPPWCDBOptionalAllTypesObject(CPPWCDBOptionalAllTypesObject&& other);
    CPPWCDBOptionalAllTypesObject();
    ~CPPWCDBOptionalAllTypesObject();
};
