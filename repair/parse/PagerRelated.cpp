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
#include <WCDB/Pager.hpp>
#include <WCDB/PagerRelated.hpp>

namespace WCDB {

namespace Repair {

PagerRelated::PagerRelated(Pager *pager) : m_pager(pager)
{
    WCTInnerAssert(m_pager != nullptr);
}

PagerRelated::PagerRelated(PagerRelated &&other) : m_pager(other.m_pager)
{
}

PagerRelated &PagerRelated::operator=(PagerRelated &&other)
{
    m_pager = other.m_pager;
    return *this;
}

void PagerRelated::setError(Error &&error)
{
    m_pager->setError(std::move(error));
}

void PagerRelated::assignWithSharedThreadedError()
{
    m_pager->assignWithSharedThreadedError();
}

void PagerRelated::markPagerAsCorrupted(int page, const std::string &message)
{
    m_pager->markAsCorrupted(page, message);
}

} //namespace Repair

} //namespace WCDB
