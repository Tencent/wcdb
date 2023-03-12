// ___FILEHEADER___

import Foundation
import WCDB

class ___VARIABLE_productName___: WCDB.Tokenizer {

    required init(args: [String]) {
    }

    func load(input: UnsafePointer<Int8>?, length: Int, flags: Int/* flags is only used in FTS5 */) {
    }

    func nextToken(ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>,
                   pnBytes: UnsafeMutablePointer<Int32>,
                   piStart: UnsafeMutablePointer<Int32>,
                   piEnd: UnsafeMutablePointer<Int32>,
                   pFlags: UnsafeMutablePointer<Int32>?,// pFlags is only used in FTS5
                   piPosition: UnsafeMutablePointer<Int32>?// iPosition is only used in FTS3/4
    ) -> WCDB.TokenizerErrorCode {
        if /* done */ {
            return .Done
        }
        return.OK
    }
}
