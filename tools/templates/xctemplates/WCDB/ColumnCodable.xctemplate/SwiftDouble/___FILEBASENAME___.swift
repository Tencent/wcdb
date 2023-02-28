// ___FILEHEADER___

import Foundation
import WCDBSwift

class ___VARIABLE_productName___: WCDBSwift.ColumnCodable, WCDBSwift.LiteralValueConvertible, WCDBSwift.ExpressionCanBeOperated {
    static var columnType: ColumnType {
        return .float
    }

    required init?(with value: FundamentalValue) {
        /* Init ___VARIABLE_productName___ From value.doubleValue */
    }

    func archivedValue() -> FundamentalValue {
        return FundamentalValue(<#Double#>)
    }
}
