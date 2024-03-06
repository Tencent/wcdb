//
// Created by qiuwenchen on 2022/11/5.
//

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

public enum TokenizerErrorCode {
    case OK
    case Done
    case NoMemory
    internal var cValue: Int32 {
        switch self {
        case .OK:
            return WCDBTokenizerErrorCodeOK
        case .Done:
            return WCDBTokenizerErrorCodeDone
        case .NoMemory:
            return WCDBTokenizerErrorCodeNoMemory
        }
    }
}

public protocol Tokenizer: AnyObject {
    init(args: [String])
    func load(input: UnsafePointer<Int8>?, length: Int, flags: Int/* flags is only used in FTS5 */)
    func nextToken(ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>,
                   pnBytes: UnsafeMutablePointer<Int32>,
                   piStart: UnsafeMutablePointer<Int32>,
                   piEnd: UnsafeMutablePointer<Int32>,
                   pFlags: UnsafeMutablePointer<Int32>?, // pFlags is only used in FTS5
                   piPosition: UnsafeMutablePointer<Int32>? // iPosition is only used in FTS3/4
    ) -> TokenizerErrorCode
}

internal final class FTSBridge {

    static func initializeCPPAPI() {
        WCDBSwiftTokenizerCreate = {
            (typeId: Int32, argc: Int32, argv: UnsafePointer<UnsafePointer<Int8>>?) -> UnsafeMutableRawPointer? in
            let type = FTSBridge.getTokenizer(of: typeId)
            var args: [String] = []
            if argc > 0, let argv = argv {
                for i in 0..<argc {
                    args.append(String(cString: argv.advanced(by: Int(i)).pointee))
                }
            }
            let tokenizer = type.init(args: args)
            return ObjectBridge.getUntypeSwiftObject(tokenizer)
        }
        WCDBSwiftTokenizerLoadInput = {
            (obj: UnsafeMutableRawPointer, input: UnsafePointer<Int8>?, length: Int32, flag: Int32) -> Void in
            let tokenizer: Tokenizer? = ObjectBridge.extractTypedSwiftObject(obj)
            guard let tokenizer = tokenizer else {
                return
            }
            tokenizer.load(input: input, length: Int(length), flags: Int(flag))
        }
        WCDBSwiftTokenizerNextToken = {
            (obj: UnsafeMutableRawPointer,
             ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>,
             pnBytes: UnsafeMutablePointer<Int32>,
             piStart: UnsafeMutablePointer<Int32>,
             piEnd: UnsafeMutablePointer<Int32>,
             pFlags: UnsafeMutablePointer<Int32>?,
             piPosition: UnsafeMutablePointer<Int32>?) -> Int32 in
            let tokenizer: Tokenizer? = ObjectBridge.extractTypedSwiftObject(obj)
            guard let tokenizer = tokenizer else {
                return WCDBTokenizerErrorCodeNoMemory
            }
            return tokenizer.nextToken(ppToken: ppToken, pnBytes: pnBytes, piStart: piStart, piEnd: piEnd, pFlags: pFlags, piPosition: piPosition).cValue
        }
    }

    static func getTokenizer(of typeId: Int32) -> Tokenizer.Type {
        return tokenizerTypes[Int(typeId)] as! Tokenizer.Type
    }

    static func register<TokenizerType: Tokenizer>(tokenizer: TokenizerType.Type, of name: String, of version: FTSVersion) {
        DispatchQueue.once(name: "Once.FTSBridge", {
            FTSBridge.initializeCPPAPI()
        })
        mutex.lock(); defer { mutex.unlock() }
        tokenizerTypes[nextTypeId] = TokenizerType.self
        WCDBCoreRegisterFTSTokenizer(name.cString, Int32(nextTypeId), version.cValue)
        nextTypeId += 1
        assert(nextTypeId < WCDBMaxSwiftTokenizerTypeId, "The count of tokenizer exceeds the maximum allowed.")
    }

    private static var tokenizerTypes: [Any?] = [Any?](repeating: nil, count: Int(WCDBMaxSwiftTokenizerTypeId))
    private static var nextTypeId = 0
    private static var mutex = Mutex()
}
