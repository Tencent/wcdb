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
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

protocol Named {
    static var name: String {get}
    var className: String {get}
}

extension Named {
    static var name: String {
        return String(describing: Self.self)
    }
    var className: String {
        return Self.name
    }
}

extension URL {
    func urlByAppendingPathComponent(_ pathComponent: String) -> URL {
        var url = self
        url.appendPathComponent(pathComponent)
        return url
    }
}

extension Database {
    var firstMaterialPath: String {
        return self.path + "-first.material"
    }
    var lastMaterialPath: String {
        return self.path + "-last.material"
    }
    var factoryPath: String {
        return self.path + ".factory"
    }
    func corruptHeader() throws {
        try self.close {
            try self.truncateCheckpoint()
            let fileHandle = FileHandle(forWritingAtPath: self.path)
            let data = Data(count: 100)
            fileHandle?.seek(toFileOffset: 0)
            fileHandle?.write(data)
        }
    }
}

extension FileManager {
    func fileSize(of path: String) -> UInt64 {
        let attributeDic = try? self.attributesOfItem(atPath: path)
        return attributeDic?[.size] as? UInt64 ?? UInt64(0)
    }
}
