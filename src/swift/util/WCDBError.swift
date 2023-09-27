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
import WCDB_Private

public final class WCDBError: Swift.Error, CustomStringConvertible {
    public enum Code: Int {
        case OK = 0
        case Error = 1
        case Internal = 2
        case Permission = 3
        case Abort = 4
        case Busy = 5
        case Locked = 6
        case NoMemory = 7
        case Readonly = 8
        case Interrupt = 9
        case IOError = 10
        case Corrupt = 11
        case NotFound = 12
        case Full = 13
        case CantOpen = 14
        case `Protocol` = 15
        case Empty = 16
        case Schema = 17
        case Exceed = 18
        case Constraint = 19
        case Mismatch = 20
        case Misuse = 21
        case NoLargeFileSupport = 22
        case Authorization = 23
        case Format = 24
        case Range = 25
        case NotADatabase = 26
        case Notice = 27
        case Warning = 28
        case Row = 100
        case Done = 101
    }

    public enum ExtendCode: Int {
        case ErrorMissingCollseq = 257       // Code.Error | (1 << 8)
        case ErrorRetry = 513                // Code.Error | (2 << 8)
        case ErrorSnapshot = 769             // Code.Error | (3 << 8)
        case IOErrorRead = 266               // Code.IOError | (1 << 8)
        case IOErrorShortRead = 522          // Code.IOError | (2 << 8)
        case IOErrorWrite = 778              // Code.IOError | (3 << 8)
        case IOErrorFsync = 1034             // Code.IOError | (4 << 8)
        case IOErrorDirFsync = 1290          // Code.IOError | (5 << 8)
        case IOErrorTruncate = 1546          // Code.IOError | (6 << 8)
        case IOErrorFstat = 1802             // Code.IOError | (7 << 8)
        case IOErrorUnlock = 2058            // Code.IOError | (8 << 8)
        case IOErrorRdlock = 2314            // Code.IOError | (9 << 8)
        case IOErrorDelete = 2570            // Code.IOError | (10 << 8)
        case IOErrorBlocked = 2826           // Code.IOError | (11 << 8)
        case IOErrorNoMemory = 3082          // Code.IOError | (12 << 8)
        case IOErrorAccess = 3338            // Code.IOError | (13 << 8)
        case IOErrorCheckReservedLock = 3594 // Code.IOError | (14 << 8)
        case IOErrorLock = 3850              // Code.IOError | (15 << 8)
        case IOErrorClose = 4106             // Code.IOError | (16 << 8)
        case IOErrorDirClose = 4362          // Code.IOError | (17 << 8)
        case IOErrorShmOpen = 4618           // Code.IOError | (18 << 8)
        case IOErrorShmSize = 4874           // Code.IOError | (19 << 8)
        case IOErrorShmLock = 5130           // Code.IOError | (20 << 8)
        case IOErrorShmMap = 5386            // Code.IOError | (21 << 8)
        case IOErrorSeek = 5642              // Code.IOError | (22 << 8)
        case IOErrorDeleteNoEntry = 5898     // Code.IOError | (23 << 8)
        case IOErrorMmap = 6154              // Code.IOError | (24 << 8)
        case IOErrorGetTempPath = 6410       // Code.IOError | (25 << 8)
        case IOErrorConvPath = 6666          // Code.IOError | (26 << 8)
        case IOErrorVnode = 6922             // Code.IOError | (27 << 8)
        case IOErrorAuthorization = 7178     // Code.IOError | (28 << 8)
        case IOErrorBeginAtomic = 7434       // Code.IOError | (29 << 8)
        case IOErrorCommitAtomic = 7690      // Code.IOError | (30 << 8)
        case IOErrorRollbackAtomic = 7946    // Code.IOError | (31 << 8)
        case LockedSharedCache = 262         // Code.Locked | (1 << 8)
        case LockedVirtualTable = 518        // Code.Locked | (2 << 8)
        case BusyRecovery = 261              // Code.Busy | (1 << 8)
        case BusySnapshot = 517              // Code.Busy | (2 << 8)
        case CantOpenNoTempDir = 270         // Code.CantOpen | (1 << 8)
        case CantOpenIsDir = 526             // Code.CantOpen | (2 << 8)
        case CantOpenFullPath = 782          // Code.CantOpen | (3 << 8)
        case CantOpenConvPath = 1038         // Code.CantOpen | (4 << 8)
        case CantOpenDirtyWal = 1294         // Code.CantOpen | (5 << 8)
        case CorruptVirtualTable = 267       // Code.Corrupt | (1 << 8)
        case CorruptSequence = 523           // Code.Corrupt | (2 << 8)
        case ReadonlyRecovery = 264          // Code.Readonly | (1 << 8)
        case ReadonlyCantLock = 520          // Code.Readonly | (2 << 8)
        case ReadonlyRollback = 776          // Code.Readonly | (3 << 8)
        case ReadonlyDatabaseMoved = 1032    // Code.Readonly | (4 << 8)
        case ReadonlyCantInit = 1288         // Code.Readonly | (5 << 8)
        case ReadonlyDirectory = 1544        // Code.Readonly | (6 << 8)
        case AbortRollback = 516             // Code.Abort | (2 << 8)
        case ConstraintCheck = 275           // Code.Constraint | (1 << 8)
        case ConstraintCommitHook = 531      // Code.Constraint | (2 << 8)
        case ConstraintForeignKey = 787      // Code.Constraint | (3 << 8)
        case ConstraintFunction = 1043       // Code.Constraint | (4 << 8)
        case ConstraintNotNull = 1299        // Code.Constraint | (5 << 8)
        case ConstraintPrimaryKey = 1555     // Code.Constraint | (6 << 8)
        case ConstraintTrigger = 1811        // Code.Constraint | (7 << 8)
        case ConstraintUnique = 2067         // Code.Constraint | (8 << 8)
        case ConstraintVirtualTable = 2323   // Code.Constraint | (9 << 8)
        case ConstraintRowID = 2579          // Code.Constraint | (10 << 8)
        case NoticeRecoverWal = 283          // Code.Notice | (1 << 8)
        case NoticeRecoverRollback = 539     // Code.Notice | (2 << 8)
        case WarningAutoIndex = 284          // Code.Warning | (1 << 8)
        case AuthorizationUser = 279         // Code.Authorization | (1 << 8)
        case OKLoadPermanently = 256         // Code.OK | (1 << 8)
    }

    public enum Level: Int, CustomStringConvertible {
        case Ignore = 1
        case Debug = 2
        case Notice = 3
        case Warning = 4
        case Error = 5 // Only for the errors that will cause api to return false.
        case Fatal = 6 // Application should abort.
        public var description: String {
            switch self {
            case .Ignore:
                return "IGNORE"
            case .Debug:
                return "DEBUG"
            case .Notice:
                return "NOTICE"
            case .Warning:
                return "WARNING"
            case .Error:
                return "ERROR"
            case .Fatal:
                return "FATAL"
            }
        }
    }

    public enum Key: Int, CustomStringConvertible {
        case tag = 1
        case path = 2
        case type = 3
        case source = 4
        case SQL = 5
        case extendedCode = 6
        case message = 7
        case invalidKey = 8

        init(stringKey: String) {
            switch stringKey {
            case "Tag":
                self = .tag
            case "Path":
                self = .path
            case "Type":
                self = .type
            case "Source":
                self = .source
            case "ExtCode":
                self = .extendedCode
            case "SQL":
                self = .SQL
            case "Message":
                self = .message
            default:
                self = .invalidKey
            }
        }
        public var description: String {
            switch self {
            case .tag:
                return "Tag"
            case .path:
                return "Path"
            case .type:
                return "Type"
            case .source:
                return "Source"
            case .extendedCode:
                return "ExtCode"
            case .message:
                return "Message"
            case .SQL:
                return "SQL"
            case .invalidKey:
                return "InvalidKey"
            }
        }
    }

    public let level: WCDBError.Level
    public let code: WCDBError.Code

    public typealias Infos = [WCDBError.Key: Value]
    public let infos: Infos

    public typealias ExtInfos = [String: Value]
    public let extInfos: ExtInfos

    internal convenience init(level: WCDBError.Level, code: WCDBError.Code, infos: Infos) {
        self.init(level: level, code: code, infos: infos, extInfos: ExtInfos())
    }

    internal init(level: WCDBError.Level, code: WCDBError.Code, infos: Infos, extInfos: ExtInfos) {
        self.infos = infos
        self.level = level
        self.code = code
        self.extInfos = extInfos
    }

    public var tag: Tag? {
        return Tag(infos[.tag]?.intValue ?? 0)
    }

    public var extendedCode: WCDBError.ExtendCode? {
        if let code = infos[Key.extendedCode]?.intValue {
            return ExtendCode(rawValue: Int(code))
        }
        return nil
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
        return "[\(level)] Code:\(code) \(infos.joined({ "\($0.description):\($1.stringValue)" })) \(extInfos.joined({ "\($0):\($1.stringValue)" }))"
    }
}
