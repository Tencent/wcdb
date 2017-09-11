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

#import <Foundation/Foundation.h>
#import <WCDB/WCTBaseAccessor.h>
#import <functional>
#import <type_traits>

template <typename T, typename Enable = void>
struct ColumnIsObjCType : public std::false_type {
};
template <typename T>
struct ColumnIsObjCType<
    T,
    typename std::enable_if<std::is_convertible<T, id>::value>::type>
    : public std::true_type {
};

class WCTObjCAccessor : public WCTBaseAccessor {
public:
    using OCType = id; //NSData*, NSString*, NSNumber*
    using Setter = void (^)(InstanceType, OCType);
    using Getter = OCType (^)(InstanceType);

    WCTObjCAccessor(Getter getter, Setter setter)
        : getObject(getter), setObject(setter)
    {
    }

    virtual WCTAccessorType getAccessorType() const override
    {
        return WCTAccessorObjC;
    }

    const Setter setObject;
    const Getter getObject;
};
