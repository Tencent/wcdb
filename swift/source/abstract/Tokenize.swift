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

public protocol TokenizerInfoBase: class {
    init(withArgc argc: Int32, andArgv argv: UnsafePointer<UnsafePointer<Int8>?>?)
}

public protocol CursorInfoBase: class {
    init(withInput pInput: UnsafePointer<Int8>?,
         count: Int32,
         tokenizerInfo: TokenizerInfoBase)

    func step(pToken: inout UnsafePointer<Int8>?,
              count: inout Int32,
              startOffset: inout Int32,
              endOffset: inout Int32, position: inout Int32) -> Int32
}

public protocol ModuleBase: class {
    static var name: String {get}

    static var module: sqlite3_tokenizer_module {get}

    static var address: Data {get}

    static func create(argc: Int32,
                       argv: UnsafePointer<UnsafePointer<Int8>?>?,
                       ppTokenizer: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer>?>?) -> Int32

    static func destroy(pTokenizer optionalTokenizerPointer: UnsafeMutablePointer<sqlite3_tokenizer>?) -> Int32

    static func open(pTokenizer: UnsafeMutablePointer<sqlite3_tokenizer>?,
                     pInput: UnsafePointer<Int8>?,
                     nBytes: Int32,
                     ppCursor: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer_cursor>?>?) -> Int32

    static func close(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?) -> Int32

    static func next(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?,
                     ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>?,
                     pnBytes: UnsafeMutablePointer<Int32>?,
                     piStartOffset: UnsafeMutablePointer<Int32>?,
                     piEndOffset: UnsafeMutablePointer<Int32>?,
                     piPosition: UnsafeMutablePointer<Int32>?) -> Int32
}

public protocol Module: ModuleBase {
    associatedtype TokenizerInfo: TokenizerInfoBase

    associatedtype CursorInfo: CursorInfoBase
}

extension Module {
    public static func create(argc: Int32,
                              argv: UnsafePointer<UnsafePointer<Int8>?>?,
                              ppTokenizer: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer>?>?) -> Int32 {
        let tokenizerSize = MemoryLayout<Tokenizer>.size
        let optionalTokenizerRawPointer = sqlite3_malloc(Int32(tokenizerSize))
        guard let tokenizerRawPointer = optionalTokenizerRawPointer else {
            return SQLITE_NOMEM
        }
        memset(tokenizerRawPointer, 0, tokenizerSize)

        let info = TokenizerInfo(withArgc: argc, andArgv: argv)
        let tokenizerPointer = tokenizerRawPointer.assumingMemoryBound(to: Tokenizer.self)
        let opaqueInfo = Unmanaged.passRetained(info).toOpaque()
        tokenizerPointer.pointee.info = opaqueInfo

        ppTokenizer!.pointee = tokenizerRawPointer.assumingMemoryBound(to: sqlite3_tokenizer.self)

        return SQLITE_OK
    }

    public static func destroy(pTokenizer optionalTokenizerPointer: UnsafeMutablePointer<sqlite3_tokenizer>?) -> Int32 {
        if let tokenizerRawPointer = UnsafeMutableRawPointer(optionalTokenizerPointer) {
            let tokenizerPointer = tokenizerRawPointer.assumingMemoryBound(to: Tokenizer.self)
            Unmanaged<AnyObject>.fromOpaque(tokenizerPointer.pointee.info).release()
            sqlite3_free(tokenizerRawPointer)
        }
        return SQLITE_OK
    }

    public static func open(pTokenizer: UnsafeMutablePointer<sqlite3_tokenizer>?,
                            pInput: UnsafePointer<Int8>?,
                            nBytes: Int32,
                            ppCursor: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer_cursor>?>?) -> Int32 {
        var bytes: Int32 = nBytes
        if pInput == nil {
            bytes = 0
        }else if bytes < 0 {
            bytes = Int32(strlen(pInput!))
        }

        let cursorSize = MemoryLayout<Cursor>.size
        let optionalCursorRawPointer = sqlite3_malloc(Int32(cursorSize))
        guard let cursorRawPointer = optionalCursorRawPointer else {
            return SQLITE_NOMEM
        }
        memset(cursorRawPointer, 0, cursorSize)

        let tokenizerPointer = UnsafeMutableRawPointer(pTokenizer!).assumingMemoryBound(to: Tokenizer.self)
        let cursorPointer = cursorRawPointer.assumingMemoryBound(to: Cursor.self)
        let tokenizerInfo = Unmanaged<TokenizerInfo>.fromOpaque(tokenizerPointer.pointee.info).takeUnretainedValue()
        let cursorInfo = CursorInfo(withInput: pInput, count: bytes, tokenizerInfo: tokenizerInfo)
        cursorPointer.pointee.info = Unmanaged.passRetained(cursorInfo).toOpaque()

        ppCursor!.pointee = cursorRawPointer.assumingMemoryBound(to: sqlite3_tokenizer_cursor.self)

        return SQLITE_OK
    }

    public static func close(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?) -> Int32 {
        if let cursorRawPointer = UnsafeMutableRawPointer(optionalCursorPointer) {
            let cursorPointer = cursorRawPointer.assumingMemoryBound(to: Cursor.self)
            Unmanaged<AnyObject>.fromOpaque(cursorPointer.pointee.info).release()
            sqlite3_free(cursorRawPointer)
        }
        return SQLITE_OK
    }

    public static func next(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?,
                            ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>?,
                            pnBytes: UnsafeMutablePointer<Int32>?,
                            piStartOffset: UnsafeMutablePointer<Int32>?,
                            piEndOffset: UnsafeMutablePointer<Int32>?,
                            piPosition: UnsafeMutablePointer<Int32>?) -> Int32 {
        if let cursorRawPointer = UnsafeMutableRawPointer(optionalCursorPointer) {
            let cursorPointer = cursorRawPointer.assumingMemoryBound(to: Cursor.self)
            let cursorInfoPointer = Unmanaged<CursorInfo>.fromOpaque(cursorPointer.pointee.info)

            let rc = cursorInfoPointer.takeUnretainedValue().step(pToken: &ppToken!.pointee,
                                                                  count: &pnBytes!.pointee,
                                                                  startOffset: &piStartOffset!.pointee,
                                                                  endOffset: &piEndOffset!.pointee,
                                                                  position: &piPosition!.pointee)
            return rc
        }
        return SQLITE_NOMEM
    }
}

public struct Tokenize {
    let module: ModuleBase.Type
}

public struct FTSModule {
    public let name: String
}

extension FTSModule {
    public static let fts3 = FTSModule(name: "fts3")
}
