//___FILEHEADER___

import Foundation
import WCDB

class ___VARIABLE_productName___: WCDB.ColumnCodable {
    typealias FundamentalType = Data

    required init?(with value: Data) {
        /* <#Init ___VARIABLE_productName___ From Data#> */
    }

    func archivedValue() -> Data? {
        return /* <#Archive ___VARIABLE_productName___ To Data#> */
    }
}
