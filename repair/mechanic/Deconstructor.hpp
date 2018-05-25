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

#ifndef Deconstructor_hpp
#define Deconstructor_hpp

#include <WCDB/MasterCrawler.hpp>
#include <WCDB/Material.hpp>
#include <string>
#include <vector>

namespace WCDB {

namespace Repair {

class Deconstructor : public MasterCrawler {
#pragma mark - Initialize
public:
    Deconstructor(const std::string &path);

#pragma mark - Deconstructor
public:
    virtual bool work();

    typedef std::function<bool(const std::string &)> Filter;
    void filter(const Filter &shouldTableDeconstructed);

    const Material &getMaterial() const;

protected:
    Filter m_filter;
    Material m_material;
    int m_height;

#pragma mark - Crawlable
protected:
    bool onCellCrawled(const Cell &cell) override;
    bool onPageCrawled(const Page &page, int height) override;
};

} //namespace Repair

} //namespace WCDB

#endif /* Deconstructor_hpp */
