//
// Created by qiuwenchen on 2023/10/31.
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

class CPPSharedPtrAllTypesObject {
public:
    WCDB::StringView type;

    // Enum
    std::shared_ptr<EnumType> enumValue;
    std::shared_ptr<EnumClassType> enumClassValue;
    std::shared_ptr<LiteralEnum> literalEnumValue;

    // Bool
    std::shared_ptr<bool> trueOrFalseValue;

    // Integer
    std::shared_ptr<char> charValue;
    std::shared_ptr<unsigned char> unsignedCharValue;
    std::shared_ptr<short> shortValue;
    std::shared_ptr<unsigned short> unsignedShortValue;
    std::shared_ptr<int> intValue;
    std::shared_ptr<unsigned int> unsignedIntValue;
    std::shared_ptr<int32_t> int32Value;
    std::shared_ptr<int64_t> int64Value;
    std::shared_ptr<uint32_t> uint32Value;
    std::shared_ptr<uint64_t> uint64Value;

    // Float
    std::shared_ptr<float> floatValue;
    std::shared_ptr<double> doubleValue;

    // String
    std::shared_ptr<const char*> constCharpValue;
    std::shared_ptr<char*> charpValue;
    std::shared_ptr<std::string> stdStringValue;
    std::shared_ptr<WCDB::UnsafeStringView> unsafeStringViewValue;
    std::shared_ptr<WCDB::StringView> stringViewValue;

    // BLOB
    std::shared_ptr<std::vector<unsigned char>> blobValue;
    std::shared_ptr<WCDB::UnsafeData> unsafeDataValue;
    std::shared_ptr<WCDB::Data> dataValue;

    WCDB_CPP_ORM_DECLARATION(CPPSharedPtrAllTypesObject)

private:
    void copyFrom(const CPPSharedPtrAllTypesObject& other);

public:
    static CPPSharedPtrAllTypesObject maxObject();
    static CPPSharedPtrAllTypesObject minObject();
    static CPPSharedPtrAllTypesObject emptyObject();
    static CPPSharedPtrAllTypesObject randomObject();

    bool operator==(const CPPSharedPtrAllTypesObject& other);
    CPPSharedPtrAllTypesObject& operator=(const CPPSharedPtrAllTypesObject& other);
    CPPSharedPtrAllTypesObject(const CPPSharedPtrAllTypesObject& other);
    CPPSharedPtrAllTypesObject(CPPSharedPtrAllTypesObject&& other);
    CPPSharedPtrAllTypesObject();
    ~CPPSharedPtrAllTypesObject();
};
