//
// Created by sanhuazhang on 2018/06/08
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

#include "Initializeable.hpp"
#include "Page.hpp"
#include "WCDBOptional.hpp"
#include "WalRelated.hpp"
#include <utility>

namespace WCDB {

namespace Repair {

class Wal;

class Frame final : public WalRelated, public Initializeable {
public:
    Frame(int frameno, Wal *wal);
    ~Frame() override;

    static constexpr const int headerSize = 24;
    const int frameno;

    int getPageNumber() const;
    Optional<Page::Type> getPageType() const;
    const std::pair<uint32_t, uint32_t> &getChecksum() const;
    std::pair<uint32_t, uint32_t>
    calculateChecksum(const std::pair<uint32_t, uint32_t> &checksum) const;
    uint32_t getTruncate() const;

protected:
    std::pair<uint32_t, uint32_t> m_checksum;
    int m_pageno;
    uint32_t m_truncate;
    MappedData m_data;

#pragma mark - Initializeable
protected:
    bool doInitialize() override final;
};

} //namespace Repair

} //namespace WCDB
