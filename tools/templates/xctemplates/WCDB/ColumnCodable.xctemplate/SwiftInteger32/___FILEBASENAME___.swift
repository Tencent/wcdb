// ___FILEHEADER___

import Foundation
import WCDBSwift

class ___VARIABLE_productName___: WCDBSwift.ColumnCodable, WCDBSwift.LiteralValueConvertible, WCDBSwift.ExpressionCanBeOperated {
    static var columnType: ColumnType {
        return .integer32
    }

    required init?(with value: FundamentalValue) {
        /* Init ___VARIABLE_productName___ From value.int32Value */
    }

    func archivedValue() -> FundamentalValue {
        return FundamentalValue(<#Int32#>)
    }
}
