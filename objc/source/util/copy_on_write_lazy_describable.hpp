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

#ifndef copy_on_write_lazy_describable_hpp
#define copy_on_write_lazy_describable_hpp

#include "copy_on_write.hpp"
#include "copy_on_write_string.hpp"

template <typename T>
class copy_on_write_lazy_describable : public copy_on_write<T> {
public:
    const copy_on_write_string &description() const
    {
        if (!m_calculated) {
            if (!this->empty()) {
                m_description.assign(calculatedDescription());
            } else {
                m_description.clear();
            }
            m_calculated = true;
        }
        return m_description;
    }

protected:
    virtual void willProbablyChange() override
    {
        m_calculated = false;
    }
    virtual copy_on_write_string calculatedDescription() const = 0;

private:
    mutable copy_on_write_string m_description;
    mutable bool m_calculated;
};

#endif /* copy_on_write_lazy_describable_hpp */
