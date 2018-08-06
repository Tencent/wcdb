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

#ifndef Frame_hpp
#define Frame_hpp

#include <WCDB/Initializeable.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/WalRelated.hpp>
#include <utility>

namespace WCDB {

namespace Repair {

class Wal;

class Frame : public WalRelated, public Initializeable {
public:
    Frame(int frameno, Wal *wal, const std::pair<uint32_t, uint32_t> &checksum);

    static constexpr const int headerSize = 24;
    const int frameno;

    int getPageNumber() const;
    std::pair<bool, Page::Type> getPageType() const;
    const std::pair<uint32_t, uint32_t> &getChecksum() const;
    uint32_t getTruncate() const;

protected:
    std::pair<uint32_t, uint32_t> m_checksum;
    int m_pageno;
    uint32_t m_truncate;
    MappedData m_data;

    std::pair<uint32_t, uint32_t>
    calculateChecksum(const MappedData &data, const std::pair<uint32_t, uint32_t> &checksum);

#pragma mark - Initializeable
protected:
    bool doInitialize() override;
};

} //namespace Repair

} //namespace WCDB

#endif /* Frame_hpp */
