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

#ifndef Describable_hpp
#define Describable_hpp

#include <WCDB/Declare.hpp>

class Describable {
public:
    virtual const std::string &getDescription() const = 0;

protected:
    static const std::string s_empty;
};

template <typename T, typename Enable = void>
class DescribableWithLang {
};

template <typename T>
class DescribableWithLang<
    T,
    typename std::enable_if<std::is_base_of<Lang, T>::value>::type>
    : public Describable {
public:
    virtual const std::string &getDescription() const
    {
        if (!m_lang.empty()) {
            return m_lang.description().get();
        }
        return Describable::s_empty;
    }
    T &getMutableLang() { return m_lang.get_or_copy(); }
    const copy_on_write_lazy_lang<T> &getLang() const { return m_lang; }

private:
    copy_on_write_lazy_lang<T> m_lang;
};

#endif /* Describable_hpp */
