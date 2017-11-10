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

class Lock{
    func lock() {
        fatalError()
    }
    
    func unlock() {
        fatalError()
    }
}

class UnfairLock: Lock {
    var unfairLock = os_unfair_lock_s()

    override func lock() {
        guard #available(iOS 10.0, macOS 10.12, *) else {
            fatalError()
        } 
        os_unfair_lock_lock(&unfairLock)
    }
    
    override func unlock() {
        guard #available(iOS 10.0, macOS 10.12, *) else {
            fatalError()
        } 
        os_unfair_lock_unlock(&unfairLock)
    }
}

class Mutex: Lock {
    var mutex: pthread_mutex_t = pthread_mutex_t()
    
    override init() {
        pthread_mutex_init(&mutex, nil)
    }
    
    deinit {
        pthread_mutex_destroy(&mutex)
    }
    
    override func lock() {
        pthread_mutex_lock(&mutex)
    }
    
    override func unlock() {
        pthread_mutex_unlock(&mutex)
    }
}

class Spin: Lock {
    let unfair: UnfairLock?
    let mutex: Mutex?
    
    override init() {
        if #available(iOS 10.0, macOS 10.12, *) {
            mutex = nil
            unfair = UnfairLock()
        }else {
            mutex = Mutex()
            unfair = nil
        }
    }
    
    override func lock() {
        if #available(iOS 10.0, macOS 10.12, *) {
            unfair!.lock()
        }else {
            mutex!.lock()
        }
    }
    
    override func unlock() {
        if #available(iOS 10.0, macOS 10.12, *) {
            unfair!.unlock()
        }else {
            mutex!.unlock()
        }
    }
}
