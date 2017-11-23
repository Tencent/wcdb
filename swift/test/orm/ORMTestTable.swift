/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import Foundation
import WCDB

class ORMTestObject: WCDB.CodableTable {
    var aBuiltinStruct: String = ""
    var anOptionalBuiltinStruct: Float? = 1.0
    var anOptionalEmptyBuiltinStruct: Int? = nil
    
    var aClass: ORMTestClassColumn = ORMTestClassColumn()
    var anOptionalClass: ORMTestClassColumn? = ORMTestClassColumn()
    var anOptionalEmptyClass: ORMTestClassColumn? = nil
    
    var aStruct: ORMTestStructColumn = ORMTestStructColumn()
    var anOptionalStruct: ORMTestStructColumn? = ORMTestStructColumn()
    var anOptionalEmptyStruct: ORMTestStructColumn? = nil
    
    var anEnum: ORMTestEnumColumn = .A
    var anOptionalEnum: ORMTestEnumColumn? = .A
    var anOptionalEmptyEnum: ORMTestEnumColumn? = nil 
    
    required init() {}

    //WCDB
    static var objectRelationalMapping: TableBinding = TableBinding(ORMTestObject.self)

    var isAutoIncrement: Bool = false
    var lastInsertedRowID: Int64 = 0
}

//WCDB
extension ORMTestObject {
    static func columnBindings() -> [AnyColumnBinding] {
        return [
            ColumnBinding(\ORMTestObject.aBuiltinStruct, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalBuiltinStruct, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalEmptyBuiltinStruct, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.aClass, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalClass, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalEmptyClass, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.aStruct, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalStruct, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalEmptyStruct, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anEnum, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalEnum, alias: "aBuiltinStruct"),
            ColumnBinding(\ORMTestObject.anOptionalEmptyEnum, alias: "aBuiltinStruct")
        ]
    }
    
//    //optional
//    static func indexBindings() -> [IndexBinding]? {
//        return [
//            IndexBinding(withSubfix: "<#subfix name of this index#>", 
//                         indexesBy: (\ORMTestObject.<#property 1#>).asIndex(orderBy: <#.Ascending or .Descending#>)),
//            IndexBinding(withSubfix: "<#subfix name of this multiple indexes#>", 
//                         indexesBy: (\ORMTestObject.<#property 2#>).asIndex(orderBy: <#.Ascending or .Descending#>), 
//                                    (\ORMTestObject.<#property 3#>).asIndex(orderBy: <#.Ascending or .Descending#>))]
//    }
    
//    //optional
//    static func constraintBindings() -> [ConstraintBinding]? {
//        return [
//            MultiUniqueBinding(named: "<#multiple unique constraint name#>",
//                               indexesBy: (\ORMTestObject.<#property 1#>).asIndex(), (\ORMTestObject.<#property 2#>).asIndex(), 
//                               onConflict: .Fail),
//            MultiPrimaryBinding(named: "<#multiple primary constraint name#>",
//                               indexesBy: (\ORMTestObject.<#property 3#>).asIndex(), (\ORMTestObject.<#property 4#>).asIndex(),
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
