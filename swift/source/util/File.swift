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

final class File {
    static func remove(files: [String]) throws {
        let fileManager = FileManager.default
        try files.forEach { (file) in
            do {
                if fileManager.fileExists(atPath: file) {
                    try FileManager.default.removeItem(atPath: file)
                }
            } catch let error as NSError {
                throw Error.reportSystemCall(operation: .remove,
                                             path: file,
                                             errno: error.code,
                                             message: error.localizedDescription)
            }
        }
    }

    static func getSize(ofFiles files: [String]) throws -> UInt64 {
        let fileManager = FileManager.default
        return try files.reduce(into: 0, { (filesSize, file) in
            do {
                filesSize += (try fileManager.attributesOfItem(atPath: file)[.size] as? UInt64) ?? 0
            } catch let error as NSError {
                throw Error.reportSystemCall(operation: .getFileSize,
                                             path: file,
                                             errno: error.code,
                                             message: error.localizedDescription)
            }
        })
    }

    static func isExists(atPath path: String) -> Bool {
        return FileManager.default.fileExists(atPath: path)
    }

    static func hardlink(atPath source: String, toPath destination: String) throws {
        do {
            try FileManager.default.linkItem(atPath: source, toPath: destination)
        } catch let error as NSError {
            throw Error.reportSystemCall(operation: .link,
                                         path: destination,
                                         errno: error.code,
                                         message: error.localizedDescription)
        }
    }

    static func createDirectoryWithIntermediateDirectories(atPath path: String) throws {
        do {
            try FileManager.default.createDirectory(atPath: path, withIntermediateDirectories: true, attributes: nil)
        } catch let error as NSError {
            throw Error.reportSystemCall(operation: .createDirectory,
                                         path: path,
                                         errno: error.code,
                                         message: error.localizedDescription)
        }
    }

#if WCDB_IOS
    static func addFirstUserAuthenticationFileProtection(atPath path: String) throws {
        let fileManager = FileManager.default
        guard fileManager.fileExists(atPath: path) else {
            return
        }
        var attributes: [FileAttributeKey: Any]!
        do {
            attributes = try fileManager.attributesOfItem(atPath: path)
        } catch let error as NSError {
            throw Error.reportSystemCall(operation: .getAttributes,
                                         path: path,
                                         errno: error.code,
                                         message: error.localizedDescription)
        }
        guard let fileProtection = attributes[.protectionKey] as? String,
            (fileProtection == FileProtectionType.completeUntilFirstUserAuthentication.rawValue
                || fileProtection == FileProtectionType.none.rawValue) else {
            return
        }
        attributes[.protectionKey] = FileProtectionType.completeUntilFirstUserAuthentication.rawValue
        do {
            try fileManager.setAttributes(attributes, ofItemAtPath: path)
        } catch let error as NSError {
            throw Error.reportSystemCall(operation: .setAttributes,
                                         path: path,
                                         errno: error.code,
                                         message: error.localizedDescription)
        }
    }
#endif //WCDB_IOS
}
