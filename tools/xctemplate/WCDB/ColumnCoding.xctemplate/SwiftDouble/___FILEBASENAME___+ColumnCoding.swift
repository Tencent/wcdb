//___FILEHEADER___

import Foundation
import WCDB

extension ___VARIABLE_productName___: WCDB.ColumnCoding {
    typealias FundamentalType = Double
    
    init?(with optionalValue: Double?) {
        guard let value = optionalValue else {
            return nil
        }
        /* <#Init ___VARIABLE_productName___ From Double#> */
    }
    
    func archivedValue() -> Double? {
        return /* <#Archive Double * To ___VARIABLE_productName___#> */
    }
}
