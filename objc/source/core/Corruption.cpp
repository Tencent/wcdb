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
#include <WCDB/FileManager.hpp>

namespace WCDB {

Corruption::Corruption(const std::string &associatedPath_)
    : associatedPath(associatedPath_)
    , m_reaction(Reaction::Custom)
    , m_handling(false)
    , m_corrupted(false)
{
}

#pragma mark - Reaction
void Corruption::setReaction(Reaction reaction)
{
    m_reaction = reaction;
}

Corruption::Reaction Corruption::getReaction() const
{
    return m_reaction;
}

#pragma mark - Notification
void Corruption::setExtraReaction(const ExtraReaction &extraReaction)
{
    m_extraReaction = extraReaction;
}

const char *Corruption::reactionName(Reaction reaction)
{
    switch (reaction) {
        case Custom:
            return "Custom";
        case Deposit:
            return "Deposit";
        case Remove:
            return "Remove";
    }
}

void Corruption::notify()
{
    if (!m_corrupted.load()) {
        return;
    }
    WCTInnerAssert(!m_identifier.empty());

    {
        Error error;
        error.level = Error::Level::Notice;
        error.setCode(Error::Code::Notice);
        error.message = "Corruption will be handled.";
        error.infos.set("Reaction", reactionName(m_reaction));
        error.infos.set("Path", associatedPath);
        error.infos.set("ExtraReaction", m_extraReaction != nullptr);
        Notifier::shared()->notify(error);
    }

    auto database = Database::databaseWithExistingPath(associatedPath);
    WCTInnerAssert(database != nullptr);
    database->blockade();
    do {
        bool succeed;
        Time identifier;
        std::tie(succeed, identifier) =
            FileManager::shared()->getFileCreatedTime(associatedPath);
        if (!succeed || identifier != m_identifier) {
            Error error;
            error.level = Error::Level::Warning;
            error.setCode(Error::Code::Warning, "Repair");
            error.message =
                "Skip corruption handling due to mismatch identifier.";
            error.infos.set("Old", m_identifier.stringify());
            error.infos.set("New", identifier.stringify());
            Notifier::shared()->notify(error);
            break;
        }

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

    } while (false);

    database->unblockade();
    m_corrupted.store(false);
}

bool Corruption::markAsCorrupted()
{
    if (m_handling.load()) {
        return false;
    }
    if (m_corrupted.load()) {
        return false;
    }
    bool succeed;
    Time identifier;
    std::tie(succeed, identifier) =
        FileManager::shared()->getFileCreatedTime(associatedPath);
    if (!succeed) {
        return false;
    }
    m_identifier = identifier;
    return !m_corrupted.exchange(true);
}

void Corruption::markAsHandling()
{
    m_handling.store(true);
    m_corrupted.store(false);
}

void Corruption::markAsHandled()
{
    m_handling.store(false);
}

} //namespace WCDB
