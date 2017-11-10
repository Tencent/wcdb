//___FILEHEADER___

import Foundation
import WCDB

extension ___VARIABLE_productName___: WCDB.ColumnCoding {
    typealias FundamentalType = Int32
    
    init?(with optionalValue: Int32?) {
        guard let value = optionalValue else {
            return nil
        }
        /* <#Init ___VARIABLE_productName___ From Int32#> */
    }
    
    func archivedValue() -> Int32? {
        return /* <#Archive Int32 * To ___VARIABLE_productName___#> */
    }
}
