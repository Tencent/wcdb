// ___FILEHEADER___

import Foundation
import WCDB

class ___VARIABLE_productName___: WCDB.TableCodable {
    // Your own properties
    var variable1: Int = 0
    var variable2: String? // Optional if it would be nil in some WCDB selection.
    var variable3: Double? // Optional if it would be nil in some WCDB selection.
    let invalid: Int = 0 // An initialized constant property is not allowed.
    let unbound: Date? = nil

    enum CodingKeys: String, CodingTableKey {
        typealias Root = ___VARIABLE_productName___

        // List the properties which should be bound to table.
        case variable1 = "custom_name"
        case variable2
        case variable3

        static let objectRelationalMapping = TableBinding(CodingKeys.self) {
            // Column constraints for primary key, unique, not null, default value and so on. It is optional.
            // BindColumnConstraint(.variable1, isPrimary: true, isAutoIncrement: true)
            // BindColumnConstraint(.variable2, isUnique: true)

            // Index bindings. It is optional.
            // BindIndex(.variable2, namedWith: "_index")

            // Table constraints for multi-primary, multi-unique and so on. It is optional.
            // BindMultiPrimary(.variable2, .variable3)

            // Virtual table binding for FTS and so on. It is optional.
            // BindVirtualTable(withModule: .FTS5, and: BuiltinTokenizer.Verbatim)
        }
    }

    // Properties below are needed when the primary key is auto-increment.
    // var isAutoIncrement: Bool = false
    // var lastInsertedRowID: Int64 = 0
}
