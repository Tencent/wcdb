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

import Foundation

final class ThreadLocal<Value> {
    private final class Wrapper: RawRepresentable {
        typealias RawValue = Value
        var rawValue: RawValue
        init(rawValue: RawValue) {
            self.rawValue = rawValue
        }
    }

    private var key = pthread_key_t()
    private let defaultValue: Value

    init(defaultTo defaultValue: Value) {
        self.defaultValue = defaultValue
        pthread_key_create(&key, {
            Unmanaged<AnyObject>.fromOpaque($0).release()
        })
    }

    deinit {
        pthread_key_delete(key)
    }

    var value: Value {
        get {
            guard let pointer = pthread_getspecific(key) else {
                return defaultValue
            }
            return Unmanaged<Wrapper>.fromOpaque(pointer).takeUnretainedValue().rawValue
        }
        set {
            if let pointer = pthread_getspecific(key) {
                Unmanaged<AnyObject>.fromOpaque(pointer).release()
            }
            pthread_setspecific(key, Unmanaged.passRetained(Wrapper(rawValue: newValue)).toOpaque())
        }
    }
}
