//___FILEHEADER___

import Foundation
import WCDB

class ___VARIABLE_productName___: WCDB.TableCoding {
    //Your own properties
    
    required init() {
        <#required init#>
    }

    //WCDB
    static var objectRelationalMapping: TableBinding = TableBinding(___VARIABLE_productName___.self)

    var isAutoIncrement: Bool = false
    var lastInsertedRowID: Int64 = 0
}

//WCDB
extension ___VARIABLE_productName___ {
    static func columnBindings() -> [AnyColumnBinding] {
        return [
            ColumnBinding(\___VARIABLE_productName___.<#primary property#>, isPrimary: <#true or false#>, orderBy: <#.Ascending or .Descending#>, isAutoIncrement: <#true or false#>),
            ColumnBinding(\___VARIABLE_productName___.<#property 1#>),
            ColumnBinding(\___VARIABLE_productName___.<#property 2#>, alias:"<#alias name in database#>"),
            ColumnBinding(\___VARIABLE_productName___.<#property 3#>, isNotNull: <#true or false#>),
            ColumnBinding(\___VARIABLE_productName___.<#property 4#>, isUnique: <#true or false#>),
            ColumnBinding(\___VARIABLE_productName___.<#property 5#>, defaultTo: <#default value#>)]
    }
    
//    //optional
//    static func indexBindings() -> [IndexBinding]? {
//        return [
//            IndexBinding(withSubfix: "<#subfix name of this index#>", 
//                         indexesBy: (\___VARIABLE_productName___.<#property 1#>).asIndex(orderBy: <#.Ascending or .Descending#>)),
//            IndexBinding(withSubfix: "<#subfix name of this multiple indexes#>", 
//                         indexesBy: (\___VARIABLE_productName___.<#property 2#>).asIndex(orderBy: <#.Ascending or .Descending#>), 
//                                    (\___VARIABLE_productName___.<#property 3#>).asIndex(orderBy: <#.Ascending or .Descending#>))]
//    }
    
//    //optional
//    static func constraintBindings() -> [ConstraintBinding]? {
//        return [
//            MultiUniqueBinding(named: "<#multiple unique constraint name#>",
//                               indexesBy: (\___VARIABLE_productName___.<#property 1#>).asIndex(), (\___VARIABLE_productName___.<#property 2#>).asIndex(), 
//                               onConflict: .Fail),
//            MultiPrimaryBinding(named: "<#multiple primary constraint name#>",
//                               indexesBy: (\___VARIABLE_productName___.<#property 3#>).asIndex(), (\___VARIABLE_productName___.<#property 4#>).asIndex(),
//                               onConflict: .Fail)]
//    }
    
//    //optional
//    static func virtualTableBinding() -> VirtualTableBinding? {
//        return VirtualTableBinding(withModule: "<#module name#>", and: [
//            ModuleArgument(left: "<#left argument#>", right: "<#right argument#>"),
//            ModuleArgument(withTokenize: "<#tokenize#>")
//            ])
//    }
}
