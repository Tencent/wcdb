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

Corruption::Corruption(HandlePool *pool)
: HandlePoolRelated(pool), m_reaction(Reaction::Custom), m_handling(0), m_corruptedIdentifier(0)
{
}

const std::string &Corruption::getPath() const
{
    return m_pool->path;
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
    if (m_corruptedIdentifier.load() == 0 || m_handling.load() > 0) {
        return;
    }
    if (m_reaction == Reaction::Custom && m_extraReaction == nullptr) {
        return;
    }
    auto database = Database::databaseWithExistingPath(getPath());
    {
        Error error;
        error.level = Error::Level::Notice;
        error.setCode(Error::Code::Notice);
        error.message = "Corruption will be handled.";
        Tag tag = database->getTag();
        if (tag != Tag::invalid()) {
            error.infos.set("Tag", tag);
        }
        error.infos.set("Reaction", reactionName(m_reaction));
        error.infos.set("Path", getPath());
        error.infos.set("ExtraReaction", m_extraReaction != nullptr);
        Notifier::shared()->notify(error);
    }

    WCTInnerAssert(database != nullptr);
    database->blockade();
    markAsHandling();
    bool succeed = false;
    do {
        uint32_t identifier = m_pool->getIdentifier();
        if (identifier != m_corruptedIdentifier.load()) {
            Error error;
            error.level = Error::Level::Warning;
            error.setCode(Error::Code::Warning, "Repair");
            error.message = "Skip corruption handling due to mismatched identifier.";
            Tag tag = database->getTag();
            if (tag != Tag::invalid()) {
                error.infos.set("Tag", tag);
            }
            error.infos.set("Old", m_corruptedIdentifier.load());
            error.infos.set("New", identifier);
            Notifier::shared()->notify(error);
            succeed = true;
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
        if (doExtraReaction) {
            if (m_extraReaction) {
                succeed = m_extraReaction(database);
            } else {
                succeed = true;
            }
        }
    } while (false);
    markAsHandled(succeed);
    database->unblockade();
}

bool Corruption::markAsCorrupted()
{
    if (m_handling.load() > 0 || m_corruptedIdentifier.load() != 0) {
        return false;
    }
    uint32_t identifier = m_pool->getIdentifier();
    if (identifier == 0) {
        return false;
    }
    return m_corruptedIdentifier.exchange(identifier) == 0;
}

void Corruption::markAsHandling()
{
    ++m_handling;
}

void Corruption::markAsHandled(bool succeed)
{
    if (--m_handling == 0) {
        if (succeed) {
            m_corruptedIdentifier.store(0);
        }
    }
    WCTInnerAssert(m_handling.load() >= 0);
}

bool Corruption::isCorrupted() const
{
    return m_corruptedIdentifier.load() != 0;
}

} //namespace WCDB
