//
// Created by qiuwenchen on 2023/11/19.
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

#include "Data.hpp"
#include "SharedThreadedErrorProne.hpp"
#include "StringView.hpp"
#include "UnsafeData.hpp"
#include "WCDBOptional.hpp"

namespace WCDB {

typedef struct ZCDict ZCDict;
typedef struct ZDDcit ZDDcit;

class ZSTDDict : public SharedThreadedErrorProne {
public:
    ZSTDDict();
    ZSTDDict(ZSTDDict&& dict);
    ~ZSTDDict();

    ZSTDDict(const ZSTDDict&) = delete;
    ZSTDDict& operator=(const ZSTDDict&) = delete;
    ZSTDDict& operator=(ZSTDDict&&);

    bool loadData(const UnsafeData& data);

    typedef uint32_t DictId;
    DictId getDictId() const;
    ZCDict* getCDict() const;
    ZDDcit* getDDict() const;

private:
    void clearDict();
    DictId m_dictId;
    ZCDict* m_cDict;
    ZDDcit* m_dDict;
};

} //namespace WCDB
