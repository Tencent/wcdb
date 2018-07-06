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

#ifndef CopyOnWrite_hpp
#define CopyOnWrite_hpp

#include <assert.h>
#include <memory>

template<typename Type, typename StorageType = Type>
class CopyOnWrite {
public:
    CopyOnWrite() : m_shared(nullptr) {}

    CopyOnWrite(const Type &t) : m_shared(new Type(t)) {}

    template<typename OtherType, typename OtherStorageType = OtherType>
    CopyOnWrite(const CopyOnWrite<OtherType, OtherStorageType> &o)
    : m_shared(std::static_pointer_cast<StorageType>(o.m_shared))
    {
    }

    void assign(const Type &t)
    {
        willProbablyChange();
        m_shared.reset(new Type(t));
    }

    template<typename OtherType, typename OtherStorageType = OtherType>
    void assign(const CopyOnWrite<OtherType, OtherStorageType> &o)
    {
        willProbablyChange();
        m_shared = std::static_pointer_cast<StorageType>(o.m_shared);
    }

    void clear()
    {
        willProbablyChange();
        m_shared = nullptr;
    }

    bool empty() const { return m_shared == nullptr; }

    template<typename OtherType>
    const OtherType *get_pointer() const
    {
        return (OtherType *) m_shared.get();
    }

    const Type *get_pointer() const { return get_pointer<Type>(); }

    template<typename OtherType>
    const OtherType &get() const
    {
        assert(!empty());
        return *(OtherType *) m_shared.get();
    }

    const Type &get() const
    {
        assert(!empty());
        return *(Type *) m_shared.get();
    }

    template<typename OtherType>
    OtherType &get_or_copy()
    {
        willProbablyChange();
        if (!m_shared.unique()) {
            if (empty()) {
                m_shared.reset(new OtherType);
            } else {
                m_shared.reset(new OtherType(*(OtherType *) m_shared.get()));
            }
        }
        return *(OtherType *) m_shared.get();
    }

    Type &get_or_copy() { return get_or_copy<Type>(); }

    bool equal(const CopyOnWrite<Type, StorageType> &other) const
    {
        return m_shared.get() == other.m_shared.get();
    }

    template<typename OtherType, typename OtherStorageType = OtherType>
    bool equal(const CopyOnWrite<OtherType, OtherStorageType> &other) const
    {
        return m_shared.get() == other.m_shared.get();
    }

protected:
    virtual void willProbablyChange(){};

private:
    std::shared_ptr<StorageType> m_shared;

    template<typename OtherType, typename OtherStorageType>
    friend class CopyOnWrite;
};

#endif /* CopyOnWrite_hpp */
