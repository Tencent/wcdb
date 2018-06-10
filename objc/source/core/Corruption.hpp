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

#ifndef Corruption_hpp
#define Corruption_hpp

#include <functional>
#include <string>

namespace WCDB {

class Database;

class Corruption {
public:
    Corruption(const std::string &associatedPath);

    const std::string associatedPath;

#pragma mark - Reaction
public:
    enum Reaction {
        Ignore = -1,
        Remove = 0,
        Deposit = 1,
    };
    void setReaction(Reaction reaction);

protected:
    Reaction m_reaction;

#pragma mark - Notification
public:
    typedef std::function<void(std::shared_ptr<Database> &)> ExtraReaction;
    void setExtraReaction(const ExtraReaction &extraReaction);

protected:
    friend class CorruptionNotifier;
    void notify();
    bool markAsCorrupted();

private:
    std::atomic<bool> m_corrupted;
    ExtraReaction m_extraReaction;
};

} //namespace WCDB

#endif /* Corruption_hpp */
