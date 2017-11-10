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

class File {
    static func remove(files: [String]) throws {
        try files.forEach { (file) in
            do {
                let fileManager = FileManager.default
                if fileManager.fileExists(atPath: file) {
                    try FileManager.default.removeItem(atPath: file)
                }
            }catch let error as NSError {
                throw Error.reportSystemCall(operation: .Remove, path: file, errno: error.code, message: error.localizedDescription)
            }
        }
    }
//    static func isExists(atPath path: String) -> Bool {
//        return FileManager.default.fileExists(atPath: path)
//    }
//    static func getFileSize(atPath path: String) -> UInt64 {
//        do {
//            //return [FileAttributeKey : Any]
//            let attr = try FileManager.default.attributesOfItem(atPath: path)
//            return attr[FileAttributeKey.size] as! UInt64
//        } catch {
//            print("Error: \(error)")
//        }
//        return 0
//    }
//    static func createHardLink(from: String, to: String) -> Bool {
//        do {
//            try FileManager.default.linkItem(atPath: from, toPath: to)
//            return true
//        } catch {
//            print("Error: \(error)")
//        }
//        return false
//    }
//    static func removeHardLink(atPath path : String) -> Bool {
//        do {
//            try FileManager.default.removeItem(atPath: path)
//            return true
//        }catch {
//            print("Error: \(error)")
//        }
//        return false
//    }
//    static func removeFile(atPath path: String) -> Error {
//        do {
//            try FileManager.default.removeItem(atPath: path)
//            return true
//        }catch {
//            print("Error: \(error)")
//        }
//        return false
//    }
//    static func createDirectory(atPath path: String) -> Error {
//        
//    }
//    
//    static func getFilesSize(atPaths paths: [String]) -> Error {
//        
//    }
//    static func removeFiles(atPaths paths: [String]) -> Error {
//        
//    }
//    static func moveFiles(atPaths paths: [String], into directory: String) -> Error {
//        
//    }
    static func createDirectoryWithIntermediateDirectories(atPath path: String) throws {
        do {
            try FileManager.default.createDirectory(atPath: path, withIntermediateDirectories: true, attributes: nil)
        }catch let error as NSError {
            throw Error.reportSystemCall(operation: .CreateDirectory, path: path, errno: error.code, message: error.localizedDescription)
        }
    }
}
