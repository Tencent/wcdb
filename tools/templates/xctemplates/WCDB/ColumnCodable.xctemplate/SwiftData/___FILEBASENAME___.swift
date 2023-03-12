// ___FILEHEADER___

import Foundation
import WCDB

class ___VARIABLE_productName___: WCDB.ColumnCodable, WCDB.LiteralValueConvertible {
    static var columnType: ColumnType {
        return .BLOB
    }

    required init?(with value: WCDB.Value) {
		/* <#Init ___VARIABLE_productName___ From value.dataValue#> */
    }

    func archivedValue() -> WCDB.Value {
        return WCDB.Value(<#Data#>)
    }
}
