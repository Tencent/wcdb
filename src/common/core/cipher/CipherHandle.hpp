//
// Created by 陈秋文 on 2023/9/9.
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

#include "Backup.hpp"
#include "InnerHandle.hpp"

namespace WCDB {

class CipherHandle final : public InnerHandle, public Repair::CipherDelegate {
public:
    CipherHandle();
    ~CipherHandle() override;

    bool execute(const Statement &statement) override final;
    bool setCipherKey(const UnsafeData &data) override final;
    Optional<StringView> tryGetSaltFromDatabase(const UnsafeStringView &path) override final;

#pragma mark - Cipher
public:
    const Error &getCipherError() const override final;
    bool openCipherInMemory() override final;
    void closeCipher() override final;
    bool isCipherDB() override final;

    void *getCipherContext() override final;
    size_t getCipherPageSize() override final;
    StringView getCipherSalt() override final;
    bool setCipherSalt(const UnsafeStringView &salt) override final;
    bool switchCipherSalt(const UnsafeStringView &salt) override final;
    bool m_isInitializing;
};

} // namespace WCDB
