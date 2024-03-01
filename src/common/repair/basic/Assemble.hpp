//
// Created by sanhuazhang on 2019/08/27
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

#include "Cipher.hpp"
#include <map>

namespace WCDB {

class Error;

namespace Repair {

class Cell;

class AssembleDelegate {
public:
    AssembleDelegate();
    virtual ~AssembleDelegate() = 0;

    virtual void setAssemblePath(const UnsafeStringView &path) = 0;
    virtual const StringView &getAssemblePath() const = 0;

    virtual bool markAsAssembling() = 0;
    virtual bool markAsAssembled() = 0;

    virtual bool markAsMilestone() = 0;
    virtual bool
    assembleTable(const UnsafeStringView &tableName, const UnsafeStringView &sql)
    = 0;
    virtual bool isAssemblingTableWithoutRowid() const = 0;
    virtual bool assembleSequence(const UnsafeStringView &tableName, int64_t sequence) = 0;
    virtual bool assembleCell(const Cell &cell) = 0;
    void markDuplicatedAsIgnorable(bool ignorable);
    virtual void markDuplicatedAsReplaceable(bool replaceable);

    virtual bool assembleSQL(const UnsafeStringView &sql) = 0;

    virtual const Error &getAssembleError() const = 0;

    virtual void suspendAssemble() = 0;
    virtual void finishAssemble() = 0;

protected:
    bool isDuplicatedIgnorable() const;
    bool isDuplicatedReplaceable() const;

private:
    bool m_duplicatedIgnorable;
    bool m_duplicatedReplaceable;
};

class AssembleDelegateHolder {
public:
    AssembleDelegateHolder();
    virtual ~AssembleDelegateHolder() = 0;
    void setAssembleDelegate(AssembleDelegate *delegate);

protected:
    AssembleDelegate *m_assembleDelegate;
};

} //namespace Repair

} //namespace WCDB
