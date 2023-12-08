//
// Created by qiuwenchen on 2023/11/26.
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

#include "DecompressFunction.hpp"
#include "Assertion.hpp"
#include "CompressionCenter.hpp"
#include "CompressionConst.hpp"
#include "WCDBError.hpp"

namespace WCDB {

DecompressFunction::DecompressFunction(void* userContext, ScalarFunctionAPI& apiObj)
: AbstractScalarFunctionObject(userContext, apiObj)
{
}

DecompressFunction::~DecompressFunction() = default;

void DecompressFunction::process(ScalarFunctionAPI& apiObj)
{
    WCTAssert(apiObj.getValueCount() == 2);
    if (apiObj.getValueCount() != 2) {
        apiObj.setErrorResult(Error::Code::Misuse,
                              StringView::formatted("Invalid parameter count for compress funciton: %d",
                                                    apiObj.getValueCount()));
        return;
    }
    ColumnType valueType = apiObj.getValueType(0);
    int type = (int) apiObj.getIntValue(1);
    CompressedType compressionType = WCDBGetCompressedType(type);
    if (compressionType <= CompressedType::None || compressionType > CompressedType::ZSTDNormal
        || valueType != ColumnType::BLOB) {
        transferValue(valueType, apiObj);
        return;
    }
    UnsafeData data = apiObj.getBlobValue(0);
    if (data.size() == 0) {
        transferValue(valueType, apiObj);
        return;
    }
    CompressionCenter::shared().decompressContent(
    data, compressionType == CompressedType::ZSTDDict, WCDBGetOriginType(type), apiObj);
}

void DecompressFunction::transferValue(ColumnType type, ScalarFunctionAPI& apiObj)
{
    switch (type) {
    case Syntax::ColumnType::Integer:
        apiObj.setIntResult(apiObj.getIntValue(0));
        break;
    case Syntax::ColumnType::Float:
        apiObj.setFloatResult(apiObj.getFloatValue(0));
        break;
    case Syntax::ColumnType::Text:
        apiObj.setTextResult(apiObj.getTextValue(0));
        break;
    case Syntax::ColumnType::BLOB:
        apiObj.setBlobResult(apiObj.getBlobValue(0));
        break;
    default:
        apiObj.setNullResult();
        break;
    }
}

} // namespace WCDB
