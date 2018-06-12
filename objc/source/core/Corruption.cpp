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

#include <WCDB/Assertion.hpp>
#include <WCDB/Corruption.hpp>
#include <WCDB/Database.hpp>
#include <sstream>

namespace WCDB {

Corruption::Corruption(const std::string &associatedPath_)
    : associatedPath(associatedPath_), m_reaction(Reaction::Remove)
{
}

#pragma mark - Reaction
void Corruption::setReaction(Reaction reaction)
{
    m_reaction = reaction;
}

#pragma mark - Notification
void Corruption::setExtraReaction(const ExtraReaction &extraReaction)
{
    m_extraReaction = extraReaction;
}

void Corruption::notify()
{
    if (!m_corrupted.load()) {
        return;
    }
    {
        Error error;
        error.level = Error::Level::Info;
        error.setCode(Error::Code::Notice);
        error.message = "Corruption will be handled.";
        error.infos.set("Reaction", m_reaction);
        error.infos.set("Path", associatedPath);
        Notifier::shared()->notify(error);
    }

    auto database = Database::databaseWithExistingPath(associatedPath);
    WCTInnerAssert(database != nullptr);
    database->blockade();
    database->close(nullptr);
    bool doExtraReaction = true;
    switch (m_reaction) {
        case Reaction::Remove:
            doExtraReaction = database->removeFiles();
            break;
        case Reaction::Deposit:
            doExtraReaction = database->deposit();
            break;
        default:
            break;
    }
    if (doExtraReaction && m_extraReaction) {
        m_extraReaction(database);
    }
    database->unblockade();
    m_corrupted.store(false);
}

bool Corruption::markAsCorrupted()
{
    return !m_corrupted.exchange(true);
}

} //namespace WCDB
