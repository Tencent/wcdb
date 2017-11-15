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

public protocol FTSTokenizerInfo: AnyObject {
    init(withArgc argc: Int32, andArgv argv: UnsafePointer<UnsafePointer<Int8>?>?) 
}

public protocol FTSCursorInfo: AnyObject {
    init(withInput pInput: UnsafePointer<Int8>?, count: Int32, tokenizerInfo: FTSTokenizerInfo)     
    
    func step(pToken: inout UnsafePointer<Int8>?, count: inout Int32, startOffset: inout Int32, endOffset: inout Int32, position: inout Int32) -> Int32 
}

public protocol FTSModuleBase {
    static var name: String {get}
    
    static var module: sqlite3_tokenizer_module {get}
    
    static var address: Data {get}
   
    static func create(argc: Int32, argv: UnsafePointer<UnsafePointer<Int8>?>?, ppTokenizer: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer>?>?) -> Int32 
    
    static func destroy(pTokenizer optionalTokenizerPointer: UnsafeMutablePointer<sqlite3_tokenizer>?) -> Int32 
    
    static func open(pTokenizer: UnsafeMutablePointer<sqlite3_tokenizer>?, pInput: UnsafePointer<Int8>?, nBytes: Int32, ppCursor: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer_cursor>?>?) -> Int32 
    
    static func close(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?) -> Int32 
    
    static func next(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?, ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>?, pnBytes: UnsafeMutablePointer<Int32>?, piStartOffset: UnsafeMutablePointer<Int32>?, piEndOffset: UnsafeMutablePointer<Int32>?, piPosition: UnsafeMutablePointer<Int32>?) -> Int32 
}

public protocol FTSModule: FTSModuleBase {
    associatedtype TokenizerInfo: FTSTokenizerInfo
    
    associatedtype CursorInfo: FTSCursorInfo
} 

extension FTSModule {
    public static func create(argc: Int32, argv: UnsafePointer<UnsafePointer<Int8>?>?, ppTokenizer: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer>?>?) -> Int32 {
        let tokenizerSize = MemoryLayout<Tokenizer>.size
        let optionalTokenizerPointer = sqlite3_malloc(Int32(tokenizerSize))
        guard let tokenizerPointer = optionalTokenizerPointer else {
            return SQLITE_NOMEM
        }  
        memset(tokenizerPointer, 0, tokenizerSize)
        
        var tokenizer = unsafeBitCast(tokenizerPointer, to: Tokenizer.self)
        let info = TokenizerInfo(withArgc: argc, andArgv: argv)
        tokenizer.info = Unmanaged.passRetained(info).toOpaque()
        
        ppTokenizer!.pointee = UnsafeMutablePointer<sqlite3_tokenizer>(&tokenizer.base)
        
        return SQLITE_OK
    }
    
    public static func destroy(pTokenizer optionalTokenizerPointer: UnsafeMutablePointer<sqlite3_tokenizer>?) -> Int32 {
        if let tokenizerPointer = optionalTokenizerPointer {
            let tokenizer = unsafeBitCast(tokenizerPointer, to: Tokenizer.self)
            Unmanaged<AnyObject>.fromOpaque(tokenizer.info).release()
            sqlite3_free(tokenizerPointer)
        }
        return SQLITE_OK
    }
    
    public static func open(pTokenizer: UnsafeMutablePointer<sqlite3_tokenizer>?, pInput: UnsafePointer<Int8>?, nBytes: Int32, ppCursor: UnsafeMutablePointer<UnsafeMutablePointer<sqlite3_tokenizer_cursor>?>?) -> Int32 {
        var bytes: Int32 = nBytes
        if pInput == nil {
            bytes = 0
        }
        if bytes < 0 {
            bytes = Int32(strlen(pInput))
        }
        
        let cursorSize = MemoryLayout<Cursor>.size
        let optionalCursorPointer = sqlite3_malloc(Int32(cursorSize))
        guard let cursorPointer = optionalCursorPointer else {
            return SQLITE_NOMEM
        }
        memset(cursorPointer, 0, cursorSize)
        
        let tokenizer = unsafeBitCast(pTokenizer, to: Tokenizer.self)
        var cursor = unsafeBitCast(cursorPointer, to: Cursor.self)
        let tokenizerInfo = Unmanaged<TokenizerInfo>.fromOpaque(tokenizer.info).takeUnretainedValue()
        let cursorInfo = CursorInfo(withInput: pInput, count: bytes, tokenizerInfo: tokenizerInfo)
        cursor.info = Unmanaged.passRetained(cursorInfo).toOpaque()
        
        ppCursor!.pointee = UnsafeMutablePointer<sqlite3_tokenizer_cursor>(&cursor.base)
        
        return SQLITE_OK
    }
    
    public static func close(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?) -> Int32 {
        if let cursorPointer = optionalCursorPointer {
            let cursor = unsafeBitCast(cursorPointer, to: Cursor.self)
            Unmanaged<AnyObject>.fromOpaque(cursor.info).release()
            sqlite3_free(cursorPointer)
        }
        return SQLITE_OK
    }
    
    public static func next(pCursor optionalCursorPointer: UnsafeMutablePointer<sqlite3_tokenizer_cursor>?, ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>?, pnBytes: UnsafeMutablePointer<Int32>?, piStartOffset: UnsafeMutablePointer<Int32>?, piEndOffset: UnsafeMutablePointer<Int32>?, piPosition: UnsafeMutablePointer<Int32>?) -> Int32 {
        if let cursorPointer = optionalCursorPointer {
            let cursor = unsafeBitCast(cursorPointer, to: Cursor.self)
            let cursorInfo = unsafeBitCast(cursor.info, to: CursorInfo.self)
            
            let rc = cursorInfo.step(pToken: &ppToken!.pointee, count: &pnBytes!.pointee, startOffset: &piStartOffset!.pointee, endOffset: &piEndOffset!.pointee, position: &piPosition!.pointee)
            return rc
        }
        return SQLITE_NOMEM
    }
}

public struct FTSDefinedModule {
    let module: FTSModuleBase.Type
}
