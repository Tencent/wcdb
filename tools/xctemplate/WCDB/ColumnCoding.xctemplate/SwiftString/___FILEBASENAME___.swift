//___FILEHEADER___

import Foundation
import WCDB

class ___VARIABLE_productName___: WCDB.ColumnCodable {
    typealias FundamentalType = String

    required init?(with value: String) {
        /* <#Init ___VARIABLE_productName___ From String#> */
    }

    func archivedValue() -> String? {
        return /* <#Archive ___VARIABLE_productName___ To String#> */
    }
}
