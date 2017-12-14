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

final class RWLock {
    var mutex = pthread_mutex_t()
    var cond = pthread_cond_t()
    var reader = 0
    var writer = 0
    var pending = 0

    init() {
        pthread_mutex_init(&mutex, nil)
        pthread_cond_init(&cond, nil)
    }

    deinit {
        pthread_cond_destroy(&cond)
        pthread_mutex_destroy(&mutex)
    }

    func lockRead() {
        pthread_mutex_lock(&mutex); defer { pthread_mutex_unlock(&mutex) }
        while writer>0 || pending>0 {
            pthread_cond_wait(&cond, &mutex)
        }
        reader += 1
    }

    func unlockRead() {
        pthread_mutex_lock(&mutex); defer { pthread_mutex_unlock(&mutex) }
        reader -= 1
        if reader == 0 {
            pthread_cond_broadcast(&cond)
        }
    }

    func lockWrite() {
        pthread_mutex_lock(&mutex); defer { pthread_mutex_unlock(&mutex) }
        pending += 1
        while writer>0||reader>0 {
            pthread_cond_wait(&cond, &mutex)
        }
        pending -= 1
        writer += 1
    }

    func unlockWrite() {
        pthread_mutex_lock(&mutex); defer { pthread_mutex_unlock(&mutex) }
        writer -= 1
        pthread_cond_broadcast(&cond)
    }

    var isWriting: Bool {
        pthread_mutex_lock(&mutex); defer { pthread_mutex_unlock(&mutex) }
        return writer>0
    }

//    var isReading: Bool {
//        pthread_mutex_lock(&mutex); defer { pthread_mutex_unlock(&mutex) }
//        return reader>0
//    }
}
