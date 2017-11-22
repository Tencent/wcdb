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

public final class ErrorValue: ExpressibleByIntegerLiteral, ExpressibleByStringLiteral {
    public enum ErrorValueType {
        case Int
        case String
    } 
    private var value: Any
    public var type: ErrorValueType

    public init(_ value: String) {
        self.value = value
        self.type = ErrorValueType.String
    }
    
    public init(_ value: Int) {
        self.value = value
        self.type = ErrorValueType.Int
    }
    
    convenience public init(integerLiteral value: Int) {
        self.init(value)
    }
    
    convenience public init(stringLiteral value: String) {
        self.init(value)
    }
    
    public var intValue: Int {
        switch type {
        case .Int:
            return value as! Int
        case .String:
            return Int(value as! String)!
        }
    }
    
    public var stringValue: String {
        switch type {
        case .Int:
            return String(value as! Int)
        case .String:
            return value as! String
        }
    }
}

public class Error: Swift.Error, CustomStringConvertible {
    public enum Key: Int, CustomStringConvertible {
        case Tag = 1
        case Operation = 2
        case ExtendedCode = 3
        case Message = 4
        case SQL = 5
        case Path = 6
        
        public var description: String {
            switch self {
            case .Tag:
                return "Tag"
            case .Operation:
                return "Operation"
            case .ExtendedCode:
                return "ExtendedCode"
            case .Message:
                return "Message"
            case .SQL:
                return "SQL"
            case .Path:
                return "Path"
            }
        }
    }
    
    public enum ErrorType: Int, CustomStringConvertible {
        case SQLite = 1
        case SystemCall = 2
        case Core = 3
        case Interface = 4
        case Abort = 5
        case Warning = 6
        case SQLiteGlobal = 7
        case Repair = 8
        
        public var description: String {
            switch self {
            case .SQLite:
                return "SQLite"
            case .SystemCall:
                return "SystemCall"
            case .Core:
                return "Core"
            case .Interface:
                return "Interface"
            case .Abort:
                return "Abort"
            case .Warning:
                return "Warning"
            case .SQLiteGlobal:
                return "SQLiteGlobal"
            case .Repair:
                return "Repair"
            }
        }
    }
    
    public enum Operation {
        public enum HandleOperation: Int {
            case Open = 1
            case Close = 2
            case Prepare = 3
            case Exec = 4
            case Step = 5
            case Finalize = 6
            case SetCipherKey = 7
            case IsTableExists = 8
        }
        
        public enum InterfaceOperation: Int {
            case HandleStatement = 1
            case Insert = 2
            case Update = 3
            case Select = 4
            case Table = 5
            case ChainCall = 6
            case Delete = 7
        }
        
        public enum CoreOperation: Int {
            case Prepare = 1
            case Exec = 2
            case Begin = 3
            case Commit = 4
            case Rollback = 5
            case GetThreadedHandle = 6
            case FlowOut = 7
            case Tokenize = 8
        }
        
        public enum SystemCallOperation: Int {
            case Remove = 257 // 1 + 1<<8 Swift not supported
            case Link = 258
            case CreateDirectory = 259
            case GetFileSize = 260
            case GetAttributes = 261
            case SetAttributes = 262
        }
        
        public enum RepairOperation: Int {
            case SaveMaster
            case LoadMaster
            case Repair
        }
        
        case Handle(HandleOperation)
        case Interface(InterfaceOperation)
        case Core(CoreOperation)
        case SystemCall(SystemCallOperation)
        case Repair(RepairOperation)
        
        public var value: Int {
            switch self {
            case .Handle(let value):
                return value.rawValue
            case .Interface(let value):
                return value.rawValue
            case .Core(let value):
                return value.rawValue
            case .SystemCall(let value):
                return value.rawValue
            case .Repair(let value):
                return value.rawValue
            }
        }
    }
    
    public enum Code {
        public enum CoreCode: Int {
            case Misuse = 1
            case Exceed = 2
        }
        
        public enum InterfaceCode: Int {
            case ORM = 1
            case Inconsistent = 2
            case Misuse = 4
        }
        
        public enum GlobalCode: Int {
            case Warning = 1
            case Abort = 2
        }
        
        case Repair(Int)
        case SQLiteGlobal(Int)
        case SystemCall(Int)
        case SQLite(Int)
        case Core(CoreCode)
        case Interface(InterfaceCode)
        case Global(GlobalCode)
        
        public var value: Int {
            switch self {
            case .Repair(let value): 
                return value
            case .SQLiteGlobal(let value): 
                return value
            case .SystemCall(let value):
                return value
            case .SQLite(let value):
                return value
            case .Core(let value):
                return value.rawValue
            case .Interface(let value):
                return value.rawValue
            case .Global(let value):
                return value.rawValue
            }
        }
    }
    
    public typealias Infos = [Error.Key:ErrorValue]
    public let infos: Infos
    
    public let type: ErrorType
    public let code: Error.Code
    
    private init(type: Error.ErrorType, code: Error.Code, infos: Infos) {
        self.infos = infos
        self.type = type
        self.code = code
    }
    
    public var description: String {
        return "Code:\(code), Type:\(type.description), \(infos.joined({ "\($0.description):\($1.stringValue)" }))"
    }
    
    static let threadedSlient = ThreadLocal<Bool>(defaultTo: false)
        
    public typealias Reporter = (Error)->Void
    static private let reporter: Atomic<Reporter> = Atomic({
        switch $0.type {
        case Error.ErrorType.SQLiteGlobal:
            debugPrint("[WCDB][DEBUG] \($0.description)")
        case Error.ErrorType.Warning:
            print("[WCDB][WARNING] \($0.description)")
        default:
            print("[WCDB][ERROR] \($0.description)")
        }
    })
    
    static public func setReporter(_ reporter: @escaping Reporter) {
        Error.reporter.assign(reporter)
    }

    private func report() {
        guard !Error.threadedSlient.value else {
            return
        }
        Error.reporter.raw(self)
    }
    
    @discardableResult
    static func report(type: ErrorType, code: Error.Code, infos: Error.Infos) -> Error {
        let error = Error(type: type, code: code, infos: infos)
        error.report()
        return error
    }
    
    @discardableResult
    static func reportSQLite(tag: Tag?, path: String, operation: Error.Operation.HandleOperation, extendedError: Int32? = nil, sql: String? = nil, rc: Int32, message: String) -> Error {
        var infos = [
            Error.Key.Operation: ErrorValue(operation.rawValue),
            Error.Key.Message: ErrorValue(message),
            Error.Key.Path: ErrorValue(path)]
        if extendedError != nil {
            infos[Error.Key.ExtendedCode] = ErrorValue(Int(extendedError!))
        }
        if sql != nil {
            infos[Error.Key.SQL] = ErrorValue(sql!)
        }
        if tag != nil {            
            infos[Error.Key.Tag] = ErrorValue(tag!)
        }
        return Error.report(type: ErrorType.SQLite, code: Error.Code.SQLite(Int(rc)), infos: infos)
    }
    
    @discardableResult
    static func reportCore(tag: Tag?, path: String, operation: Error.Operation.CoreOperation, code: Error.Code.CoreCode, message: String) -> Error {
        var infos = [
            Error.Key.Operation: ErrorValue(operation.rawValue),
            Error.Key.Message: ErrorValue(message),
            Error.Key.Path: ErrorValue(path)]
        if tag != nil {            
            infos[Error.Key.Tag] = ErrorValue(tag!)
        }
        return Error.report(type: ErrorType.Core, code: Error.Code.Core(code), infos: infos)
    }
    
    @discardableResult
    static func reportInterface(tag: Tag?, path: String, operation: Error.Operation.InterfaceOperation, code: Error.Code.InterfaceCode, message: String) -> Error {
        var infos = [
            Error.Key.Operation: ErrorValue(operation.rawValue),
            Error.Key.Message: ErrorValue(message),
            Error.Key.Path: ErrorValue(path)]
        if tag != nil {            
            infos[Error.Key.Tag] = ErrorValue(tag!)
        }
        return Error.report(type: ErrorType.Interface, code: Error.Code.Interface(code), infos: infos)
    }
    
    @discardableResult
    static func reportSystemCall(operation: Error.Operation.SystemCallOperation, path: String, errno: Int, message: String) -> Error {
        return Error.report(type: Error.ErrorType.SystemCall, code: Error.Code.SystemCall(errno), infos: [
            Error.Key.Operation: ErrorValue(operation.rawValue),
            Error.Key.Message: ErrorValue(message),
            Error.Key.Path: ErrorValue(path)
            ])
    }
    
    @discardableResult
    static func reportSQLiteGlobal(rc: Int, message: String) -> Error {
        return Error.report(type: Error.ErrorType.SQLiteGlobal, code: Error.Code.SQLiteGlobal(rc), infos: [
            Error.Key.Message: ErrorValue(message)])
    }
    
    @discardableResult
    static func reportRepair(path: String, operation: Error.Operation.RepairOperation, code: Int) -> Error {
        return Error.report(type: Error.ErrorType.Repair, code: Error.Code.Repair(code), infos: [
            Error.Key.Path: ErrorValue(path),
            Error.Key.Operation: ErrorValue(operation.rawValue)])
    }
    
    static func abort(_ message: String) -> Never {
        Error.report(type: ErrorType.Abort, code: Error.Code.Global(.Abort), infos: [
            Error.Key.Message: ErrorValue(message)])
        fatalError(message)
    }
    
    static func warning(_ message: String) {
        Error.report(type: ErrorType.Warning, code: Error.Code.Global(.Warning), infos: [
            Error.Key.Message: ErrorValue(message)])
    }
}
