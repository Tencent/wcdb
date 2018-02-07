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

#ifndef copy_on_write_lazy_lang_hpp
#define copy_on_write_lazy_lang_hpp

namespace WCDB {

namespace lang {

template <typename T>
class copy_on_write_lazy_lang : public copy_on_write_lazy_describable<T> {
protected:
    virtual copy_on_write_string calculatedDescription() const override
    {
        return this->get().SQL();
    }
};

template <typename T>
class copy_on_write_lazy_lang_list
    : public copy_on_write_lazy_describable<
          std::list<copy_on_write_lazy_lang<T>>> {
public:
    bool empty() const
    {
        return copy_on_write_lazy_describable<
                   std::list<copy_on_write_lazy_lang<T>>>::empty() ||
               this->get().empty();
    }
              
    void append(const copy_on_write_lazy_lang<T> &element)
    {
        if (!this->empty()) {
            this->get_or_copy().push_back(element);
        } else {
            this->assign({element});
        }
    }
              
  template <typename U>
  void append(const copy_on_write_lazy_lang<U> &element)
  {
      copy_on_write_lazy_lang<T> convert;
      convert.assign(element);
      if (!this->empty()) {
          this->get_or_copy().push_back(convert);
      } else {
          this->assign({convert});
      }
  }

protected:
    virtual copy_on_write_string calculatedDescription() const override
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

} // namespace lang

} // namespace WCDB

#endif /* copy_on_write_lazy_lang_hpp */
