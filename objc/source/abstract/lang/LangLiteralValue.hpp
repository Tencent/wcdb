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

#ifndef LangLiteralValue_hpp
#define LangLiteralValue_hpp

#include <WCDB/LangCommon.h>

namespace WCDB {

namespace Lang {

class LiteralValue : public Lang {
public:
    LiteralValue();

    enum class Type : int {
        NotSet,
        NumbericInteger,
        NumbericFloat,
        String,
        BLOB,
        Null,
        CurrentTime,
        CurrentDate,
        CurrentTimestamp,
    };
    Type type;
    CopyOnWriteString stringValue;
    CopyOnWriteData dataValue;

    CopyOnWriteString SQL() const override;

protected:
    static CopyOnWriteString
    stringByAntiInjecting(const CopyOnWriteString &origin);
    static CopyOnWriteString
    stringByAntiInjecting(const CopyOnWriteData &origin);
};

} // namespace Lang

} // namespace WCDB

#endif /* LangLiteralValue_hpp */
