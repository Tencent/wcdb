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

public protocol CodableTable {
    static func columnBindings() -> [AnyColumnBinding]
    static func indexBindings() -> [IndexBinding]?
    static func constraintBindings() -> [ConstraintBinding]?
    static func virtualTableBinding() -> VirtualTableBinding?
    
    var isAutoIncrement: Bool {get set}
    var lastInsertedRowID: Int64 {get set}
    
    init()
    
    static var allProperties: [Property] {get}
    
    static var any: Column {get}
    
    static func column(named: String) -> Column
    
    static var objectRelationalMapping: TableBinding {get}
    static var magicNumber: Int32 {get}
}

extension CodableTable {    
    public static var allProperties: [Property] {
        return objectRelationalMapping.allProperties
    }
    
    public static var any: Column {
        return Column.any
    }
    
    public static func column(named name: String) -> Column {
        return Column(named: name)
    }
   
    public static var magicNumber: Int32 {
        return 0x539D7C2
    }
    
    public static func indexBindings() -> [IndexBinding]? {
        return nil
    }
    
    public static func constraintBindings() -> [ConstraintBinding]? {
        return nil
    }
    
    public static func virtualTableBinding() -> VirtualTableBinding? {
        return nil
    }

    public static var objectRelationalMapping: TableBinding {
        Error.warning("It's recommended to add [objectRelationalMapping] as a stored property for class \(self) to get a better performance")
        return TableBinding.from(self)
    }
    
    public var isAutoIncrement: Bool {
        get { return false }
        set { }
    }
    public var lastInsertedRowID: Int64 {
        get { return 0 }
        set { }
    }
}

