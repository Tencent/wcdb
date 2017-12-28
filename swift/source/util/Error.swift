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

public struct ErrorValue {
    public enum ErrorValueType {
        case int
        case string
    }
    private let value: Any
    public let type: ErrorValueType

    init(_ value: String) {
        self.value = value
        self.type = .string
    }

    init(_ value: Int) {
        self.value = value
        self.type = .int
    }

    public var intValue: Int {
        switch type {
        case .int:
            return value as! Int
        case .string:
            return Int(value as! String) ?? 0
        }
    }

    public var stringValue: String {
        switch type {
        case .int:
            return String(value as! Int)
        case .string:
            return value as! String
        }
    }
}

public final class Error: Swift.Error, CustomStringConvertible {
    public enum Key: Int, CustomStringConvertible {
        case tag = 1
        case operation = 2
        case extendedCode = 3
        case message = 4
        case SQL = 5
        case path = 6

        public var description: String {
            switch self {
            case .tag:
                return "Tag"
            case .operation:
                return "Operation"
            case .extendedCode:
                return "ExtendedCode"
            case .message:
                return "Message"
            case .SQL:
                return "SQL"
            case .path:
                return "Path"
            }
        }
    }

    public enum ErrorType: Int, CustomStringConvertible {
        case sqlite = 1
        case systemCall = 2
        case core = 3
        case interface = 4
        case warning = 6
        case sqliteGlobal = 7
        case repair = 8

        public var description: String {
            switch self {
            case .sqlite:
                return "SQLite"
            case .systemCall:
                return "SystemCall"
            case .core:
                return "Core"
            case .interface:
                return "Interface"
            case .warning:
                return "Warning"
            case .sqliteGlobal:
                return "SQLiteGlobal"
            case .repair:
                return "Repair"
            }
        }
    }

    public enum Operation {
        public enum HandleOperation: Int {
            case open = 1
            case close = 2
            case prepare = 3
            case exec = 4
            case step = 5
            case finalize = 6
            case setCipherKey = 7
            case isTableExists = 8
        }

        public enum InterfaceOperation: Int {
            case handleStatement = 1
            case insert = 2
            case update = 3
            case select = 4
            case table = 5
            case chainCall = 6
            case delete = 7
        }

        public enum CoreOperation: Int {
            case prepare = 1
            case exec = 2
            case begin = 3
            case commit = 4
            case rollback = 5
            case getThreadedHandle = 6
            case flowOut = 7
            case tokenize = 8
            case encode = 9
            case decode = 10
            case getPool = 11
        }

        public enum SystemCallOperation: Int {
            case remove = 257 // 1 + 1<<8 Swift not supported
            case link = 258
            case createDirectory = 259
            case getFileSize = 260
            case getAttributes = 261
            case setAttributes = 262
        }

        public enum RepairOperation: Int {
            case saveMaster
            case loadMaster
            case repair
        }

        case handle(HandleOperation)
        case interface(InterfaceOperation)
        case core(CoreOperation)
        case systemCall(SystemCallOperation)
        case repair(RepairOperation)

        public var value: Int {
            switch self {
            case .handle(let value):
                return value.rawValue
            case .interface(let value):
                return value.rawValue
            case .core(let value):
                return value.rawValue
            case .systemCall(let value):
                return value.rawValue
            case .repair(let value):
                return value.rawValue
            }
        }
    }

    public enum Code {
        public enum CoreCode: Int {
            case misuse = 1
            case exceed = 2
        }

        public enum InterfaceCode: Int {
            case ORM = 1
            case inconsistent = 2
            case misuse = 4
        }

        public enum GlobalCode: Int {
            case warning = 1
            case abort = 2
        }

        case repair(Int)
        case sqliteGlobal(Int)
        case systemCall(Int)
        case sqlite(Int)
        case core(CoreCode)
        case interface(InterfaceCode)
        case global(GlobalCode)

        public var value: Int {
            switch self {
            case .repair(let value):
                return value
            case .sqliteGlobal(let value):
                return value
            case .systemCall(let value):
                return value
            case .sqlite(let value):
                return value
            case .core(let value):
                return value.rawValue
            case .interface(let value):
                return value.rawValue
            case .global(let value):
                return value.rawValue
            }
        }
    }

    public typealias Infos = [Error.Key: ErrorValue]
    public let infos: Infos

    public let type: ErrorType
    public let code: Error.Code

    private init(type: Error.ErrorType, code: Error.Code, infos: Infos) {
        self.infos = infos
        self.type = type
        self.code = code
    }

    public var tag: Tag? {
        return infos[.tag]?.intValue
    }

    public var operationValue: Int? {
        return infos[.operation]?.intValue
    }

    public var extendedCode: Int? {
        return infos[.extendedCode]?.intValue
    }

    public var message: String? {
        return infos[.message]?.stringValue
    }

    public var sql: String? {
        return infos[.SQL]?.stringValue
    }

    public var path: String? {
        return infos[.path]?.stringValue
    }

    public var description: String {
        return "Code:\(code), Type:\(type.description), \(infos.joined({ "\($0.description):\($1.stringValue)" }))"
    }

    static let threadedSlient = ThreadLocal<Bool>(defaultTo: false)

    public typealias Reporter = (Error) -> Void
    static private let defaultReporter: Reporter = {
        switch $0.type {
        case .sqliteGlobal:
            debugPrint("[WCDB][DEBUG] \($0.description)")
        case .warning:
            print("[WCDB][WARNING] \($0.description)")
        default:
            print("[WCDB][ERROR] \($0.description)")
        }
    }
    static private let reporter: Atomic<Reporter?> = Atomic(defaultReporter)

    static public func setReporter(_ reporter: @escaping Reporter) {
        Error.reporter.assign(reporter)
    }
    static public func setReporter(_: Void?) {
        Error.reporter.assign(nil)
    }
    static public func resetReporter() {
        Error.reporter.assign(defaultReporter)
    }

    private func report() {
        guard !Error.threadedSlient.value else {
            return
        }
        Error.reporter.raw?(self)
    }

    @discardableResult
    static func report(type: ErrorType,
                       code: Error.Code,
                       infos: Error.Infos) -> Error {
        let error = Error(type: type, code: code, infos: infos)
        error.report()
        return error
    }

    @discardableResult
    static func reportSQLite(tag: Tag?,
                             path: String,
                             operation: Error.Operation.HandleOperation,
                             extendedError: Int32? = nil,
                             sql: String? = nil,
                             code: Int32,
                             message: String) -> Error {
        var infos = [
            Error.Key.operation: ErrorValue(operation.rawValue),
            Error.Key.message: ErrorValue(message),
            Error.Key.path: ErrorValue(path)]
        if extendedError != nil {
            infos[Error.Key.extendedCode] = ErrorValue(Int(extendedError!))
        }
        if sql != nil {
            infos[Error.Key.SQL] = ErrorValue(sql!)
        }
        if tag != nil {
            infos[Error.Key.tag] = ErrorValue(tag!)
        }
        return Error.report(type: .sqlite,
                            code: .sqlite(Int(code)),
                            infos: infos)
    }

    @discardableResult
    static func reportCore(tag: Tag?,
                           path: String,
                           operation: Error.Operation.CoreOperation,
                           code: Error.Code.CoreCode, message: String) -> Error {
        var infos = [
            Error.Key.operation: ErrorValue(operation.rawValue),
            Error.Key.message: ErrorValue(message),
            Error.Key.path: ErrorValue(path)]
        if tag != nil {
            infos[Error.Key.tag] = ErrorValue(tag!)
        }
        return Error.report(type: .core,
                            code: .core(code),
                            infos: infos)
    }

    @discardableResult
    static func reportInterface(tag: Tag?,
                                path: String,
                                operation: Error.Operation.InterfaceOperation,
                                code: Error.Code.InterfaceCode, message: String) -> Error {
        var infos = [
            Error.Key.operation: ErrorValue(operation.rawValue),
            Error.Key.message: ErrorValue(message),
            Error.Key.path: ErrorValue(path)]
        if tag != nil {
            infos[Error.Key.tag] = ErrorValue(tag!)
        }
        return Error.report(type: .interface,
                            code: .interface(code),
                            infos: infos)
    }

    @discardableResult
    static func reportSystemCall(operation: Error.Operation.SystemCallOperation,
                                 path: String,
                                 errno: Int,
                                 message: String) -> Error {
        return Error.report(type: .systemCall, code: .systemCall(errno), infos: [
            Error.Key.operation: ErrorValue(operation.rawValue),
            Error.Key.message: ErrorValue(message),
            Error.Key.path: ErrorValue(path)
            ])
    }

    @discardableResult
    static func reportSQLiteGlobal(code: Int,
                                   message: String) -> Error {
        return Error.report(type: .sqliteGlobal,
                            code: .sqliteGlobal(code),
                            infos: [Error.Key.message: ErrorValue(message)]
        )
    }

    @discardableResult
    static func reportRepair(path: String,
                             operation: Error.Operation.RepairOperation,
                             code: Int) -> Error {
        return Error.report(type: .repair,
                            code: .repair(code),
                            infos: [
                                Error.Key.path: ErrorValue(path),
                                Error.Key.operation: ErrorValue(operation.rawValue)])
    }

    static func warning(_ message: String) {
        Error.report(type: .warning,
                     code: .global(.warning),
                     infos: [Error.Key.message: ErrorValue(message)])
    }
}
