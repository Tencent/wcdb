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

#ifndef Shm_hpp
#define Shm_hpp

#include <WCDB/FileHandle.hpp>
#include <WCDB/Initializeable.hpp>
#include <WCDB/WalRelated.hpp>

namespace WCDB {

namespace Repair {

class Shm : public WalRelated, public Initializeable {
public:
    Shm(Wal *wal);

    const std::string &getPath() const;

protected:
    bool doInitialize() override;
    void markAsCorrupted(const std::string &message);
    FileHandle m_fileHandle;

public:
    int getMaxFrame() const;

protected:
    static constexpr const int headerSize = 48;
    int m_maxFrame;
};

} // namespace Repair

} // namespace WCDB

#endif /* Shm_hpp */
