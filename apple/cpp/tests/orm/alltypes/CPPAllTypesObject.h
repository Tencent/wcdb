//
// Created by 陈秋文 on 2022/9/5.
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

#import <WCDB/WCDB.h>

enum EnumType {
    Min = std::numeric_limits<int>::min(),
    Zero = 0,
    Max = std::numeric_limits<int>::max(),
};

enum class EnumClassType {
    Min = std::numeric_limits<int>::min(),
    Zero = 0,
    Max = std::numeric_limits<int>::max(),
};

typedef enum {
    EnumMin = std::numeric_limits<int>::min(),
    EnumZero = 0,
    EnumMax = std::numeric_limits<int>::max(),
} LiteralEnum;

class CPPAllTypesObject {
public:
    WCDB::StringView type;

    // Enum
    EnumType enumValue;
    EnumClassType enumClassValue;
    LiteralEnum literalEnumValue;

    // Bool
    bool trueOrFalseValue;

    // Integer
    char charValue;
    unsigned char unsignedCharValue;
    short shortValue;
    unsigned short unsignedShortValue;
    int intValue;
    unsigned int unsignedIntValue;
    int32_t int32Value;
    int64_t int64Value;
    uint32_t uint32Value;
    uint64_t uint64Value;

    // Float
    float floatValue;
    double doubleValue;

    // String
    const char* constCharpValue;
    char* charpValue;
    const char constCharArrValue[6] = "hello";
    char charArrValue[100];
    std::string stdStringValue;
    WCDB::UnsafeStringView unsafeStringViewValue;
    WCDB::StringView stringViewValue;

    // BLOB
    std::vector<unsigned char> blobValue;
    WCDB::UnsafeData unsafeDataValue;
    WCDB::Data dataValue;
    const unsigned char constUnsignedCharArrValue[6] = "world";
    unsigned char unsignedCharArrValue[100];

    WCDB_CPP_ORM_DECLARATION(CPPAllTypesObject)

private:
    void copyFrom(const CPPAllTypesObject& other);

public:
    static CPPAllTypesObject maxObject();
    static CPPAllTypesObject minObject();
    static CPPAllTypesObject emptyObject();
    static CPPAllTypesObject randomObject();

    bool operator==(const CPPAllTypesObject& other);
    CPPAllTypesObject& operator=(const CPPAllTypesObject& other);
    CPPAllTypesObject(const CPPAllTypesObject& other);
    CPPAllTypesObject(CPPAllTypesObject&& other);
    CPPAllTypesObject();
    ~CPPAllTypesObject();
};
