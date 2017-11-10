//___FILEHEADER___

import Foundation
import WCDB

extension ___VARIABLE_productName___: WCDB.ColumnCoding {
    typealias FundamentalType = String
    
    init?(with optionalValue: String?) {
        guard let value = optionalValue else {
            return nil
        }
        /* <#Init ___VARIABLE_productName___ From String#> */
    }
    
    func archivedValue() -> String? {
        return /* <#Archive String * To ___VARIABLE_productName___#> */
    }
}
