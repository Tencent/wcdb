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

class Config {
    static let `default` = Config()

    let valueLength: Int = 100
    let randomSeed: Int = 0

    let readCount: Int
    let writeCount: Int
    let batchWriteCount: Int

    let tableCount: Int
    let syncWriteCount: Int

    private init() {
//        #if WCDB_IOS
//            readCount = 1000000
//            writeCount = 10000
//            batchWriteCount = 300000
//            
//            tableCount = 30000
//            syncWriteCount = 2000
//        #elseif WCDB_OSX
            readCount = 1000000
            writeCount = 100000
            batchWriteCount = 1000000

            tableCount = 50000
            syncWriteCount = 500
//        #else
//            fatalError()
//        #endif
    }
}
