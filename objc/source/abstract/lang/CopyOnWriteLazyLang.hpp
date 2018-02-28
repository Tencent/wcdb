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

#ifndef CopyOnWriteLazyLang_hpp
#define CopyOnWriteLazyLang_hpp

#include <WCDB/CopyOnWriteLazyDescribable.hpp>
#include <list>

namespace WCDB {

namespace Lang {

template <typename T>
class CopyOnWriteLazyLang : public CopyOnWriteLazyDescribable<T> {
protected:
    virtual CopyOnWriteString calculatedDescription() const override
    {
        return this->get().SQL();
    }
};

template <typename T>
class CopyOnWriteLazyLangList
    : public CopyOnWriteLazyDescribable<std::list<CopyOnWriteLazyLang<T>>> {
public:
    bool empty() const
    {
        return CopyOnWriteLazyDescribable<
                   std::list<CopyOnWriteLazyLang<T>>>::empty() ||
               this->get().empty();
    }

    void append(const CopyOnWriteLazyLang<T> &element)
    {
        if (!this->empty()) {
            this->get_or_copy().push_back(element);
        } else {
            this->assign({element});
        }
    }

    template <typename U>
    void append(const CopyOnWriteLazyLang<U> &element)
    {
        CopyOnWriteLazyLang<T> convert;
        convert.assign(element);
        if (!this->empty()) {
            this->get_or_copy().push_back(convert);
        } else {
            this->assign({convert});
        }
    }

protected:
    virtual CopyOnWriteString calculatedDescription() const override
    {
        std::string description;
        bool comma = false;
        for (const auto &element : this->get()) {
            if (comma) {
                description.append(", ");
            } else {
                comma = true;
            }
            assert(!element.empty());
            description.append(element.description().get());
        }
        return description;
    }
};

} // namespace Lang

} // namespace WCDB

#endif /* CopyOnWriteLazyLang_hpp */
