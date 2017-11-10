//___FILEHEADER___

import Foundation
import WCDB

extension ___VARIABLE_productName___: WCDB.ColumnCoding {
    typealias FundamentalType = Int64
    
    init?(with optionalValue: Int64?) {
        guard let value = optionalValue else {
            return nil
        }
        /* <#Init ___VARIABLE_productName___ From Int64#> */
    }
    
    func archivedValue() -> Int64? {
        return /* <#Archive Int64 * To ___VARIABLE_productName___#> */
    }
}
