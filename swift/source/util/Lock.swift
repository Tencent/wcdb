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

protocol Lock {
    func lock()
    func unlock()
}

final class UnfairLock: Lock {
    private var unfairLock = os_unfair_lock_s()

    func lock() {
        guard #available(iOS 10.0, macOS 10.12, *) else {
            Error.abort("")
        }
        os_unfair_lock_lock(&unfairLock)
    }

    func unlock() {
        guard #available(iOS 10.0, macOS 10.12, *) else {
            Error.abort("")
        }
        os_unfair_lock_unlock(&unfairLock)
    }
}

final class Mutex: Lock {
    private var mutex = pthread_mutex_t()

    init() {
        pthread_mutex_init(&mutex, nil)
    }

    deinit {
        pthread_mutex_destroy(&mutex)
    }

    func lock() {
        pthread_mutex_lock(&mutex)
    }

    func unlock() {
        pthread_mutex_unlock(&mutex)
    }
}

final class RecursiveMutex: Lock {
    private var mutex = pthread_mutex_t()

    init() {
        var attr = pthread_mutexattr_t()
        pthread_mutexattr_init(&attr)
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)
        pthread_mutex_init(&mutex, &attr)
    }

    deinit {
        pthread_mutex_destroy(&mutex)
    }

    func lock() {
        pthread_mutex_lock(&mutex)
    }

    func unlock() {
        pthread_mutex_unlock(&mutex)
    }
}

final class Spin: Lock {
    private let unfair: UnfairLock?
    private let mutex: Mutex?

    init() {
        if #available(iOS 10.0, macOS 10.12, *) {
            mutex = nil
            unfair = UnfairLock()
        } else {
            mutex = Mutex()
            unfair = nil
        }
    }

    func lock() {
        if #available(iOS 10.0, macOS 10.12, *) {
            unfair!.lock()
        } else {
            mutex!.lock()
        }
    }

    func unlock() {
        if #available(iOS 10.0, macOS 10.12, *) {
            unfair!.unlock()
        } else {
            mutex!.unlock()
        }
    }
}

final class ConditionLock: Lock {
    private var mutex = pthread_mutex_t()
    private var cond = pthread_cond_t()

    init() {
        pthread_mutex_init(&mutex, nil)
        pthread_cond_init(&cond, nil)
    }

    deinit {
        pthread_cond_destroy(&cond)
        pthread_mutex_destroy(&mutex)
    }

    func lock() {
        pthread_mutex_lock(&mutex)
    }

    func unlock() {
        pthread_mutex_unlock(&mutex)
    }

    func wait() {
        pthread_cond_wait(&cond, &mutex)
    }

    func signal() {
        pthread_cond_signal(&cond)
    }

    func broadcast() {
        pthread_cond_broadcast(&cond)
    }
}

extension DispatchQueue {
    static private let spin = Spin()
    static private var tracker: Set<String> = []

    static func once(name: String, _ block: () -> Void) {
        spin.lock(); defer { spin.unlock() }
        guard !tracker.contains(name) else {
            return
        }
        block()
        tracker.insert(name)
    }
}
