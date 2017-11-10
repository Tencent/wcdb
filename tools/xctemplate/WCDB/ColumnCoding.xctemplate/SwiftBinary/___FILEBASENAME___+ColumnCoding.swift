//___FILEHEADER___

import Foundation
import WCDB

extension ___VARIABLE_productName___: WCDB.ColumnCoding {
    typealias FundamentalType = Data
    
    init?(with optionalValue: Data?) {
        guard let value = optionalValue else {
            return nil
        }
        /* <#Init ___VARIABLE_productName___ From Data#> */
    }
    
    func archivedValue() -> Data? {
        return /* <#Archive Data * To ___VARIABLE_productName___#> */
    }
}
