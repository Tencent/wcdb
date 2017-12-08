//___FILEHEADER___

import Foundation
import WCDB

public class ___VARIABLE_productName___TokenizerInfo: TokenizerInfoBase {
    /* <#Your Tokenizer Info#> */

    public required init(withArgc argc: Int32, andArgv argv: UnsafePointer<UnsafePointer<Int8>?>?) {
        /* <#Init Your Tokenizer Info#> */
    }
}

public class ___VARIABLE_productName___CursorInfo: CursorInfoBase {
    /* <#Your Cursor Info#> */

    public required init(withInput pInput: UnsafePointer<Int8>?, count: Int32, tokenizerInfo: TokenizerInfoBase) {
        /* <#Init Your Cursor Info#> */
    }
    
    public func step(pToken: inout UnsafePointer<Int8>?, count: inout Int32, startOffset: inout Int32, endOffset: inout Int32, position: inout Int32) -> Int32 {
        if (/* <#Is It Done Yet ?#> */) {
            
            /* <#Your Cursor Step#> */
            
            return SQLITE_OK;
        }
        return SQLITE_DONE;

    }
}

public class ___VARIABLE_productName___Module: Module {
    public typealias TokenizerInfo = ___VARIABLE_productName___TokenizerInfo
    public typealias CursorInfo = ___VARIABLE_productName___CursorInfo
    
    public static let name = "___VARIABLE_productName___"
    
    public private(set) static var module = sqlite3_tokenizer_module(iVersion: 0, 
                                                 xCreate: { (argc, argv, ppTokenizer) -> Int32 in
                                                    return ___VARIABLE_productName___Module.create(argc: argc, argv: argv, ppTokenizer: ppTokenizer)
    }, 
                                                 xDestroy: { (pTokenizer) -> Int32 in
                                                    return ___VARIABLE_productName___Module.destroy(pTokenizer: pTokenizer)
    }, 
                                                 xOpen: { (pTokenizer, pInput, nBytes, ppCursor) -> Int32 in
                                                    return ___VARIABLE_productName___Module.open(pTokenizer: pTokenizer, pInput: pInput, nBytes: nBytes, ppCursor: ppCursor)
    }, 
                                                 xClose: { (pCursor) -> Int32 in
                                                    return ___VARIABLE_productName___Module.close(pCursor: pCursor)
    }, 
                                                 xNext: { (pCursor, ppToken, pnBytes, piStartOffset, piEndOffset, piPosition) -> Int32 in
                                                    return ___VARIABLE_productName___Module.next(pCursor: pCursor, ppToken: ppToken, pnBytes: pnBytes, piStartOffset: piStartOffset, piEndOffset: piEndOffset, piPosition: piPosition)
    }, 
                                                 xLanguageid: nil)
    
    public static let address = { () -> Data in
        var pointer = UnsafeMutableRawPointer(&module)
        return Data(bytes: &pointer, count: MemoryLayout.size(ofValue: pointer))
    }()
}

extension Tokenize {
    public static let ___VARIABLE_productName___ = Tokenize(module: ___VARIABLE_productName___Module.self)
}

