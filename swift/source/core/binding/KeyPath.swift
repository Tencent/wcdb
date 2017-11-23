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

extension WritableKeyPath: PropertyConvertible {
    public func asProperty() -> Property {
        guard let CodableTableRoot = Root.self as? CodableTable.Type else {
            Error.abort("")
        }
        return CodableTableRoot.objectRelationalMapping.property(from: self)
    }
    
    public func `in`(table: String) -> Property {
        return asProperty().`in`(table: table)
    }
}

extension WritableKeyPath: ColumnBindingRepresentable {
    public var columnBinding: AnyColumnBinding {
        return asProperty().columnBinding
    }
}

extension WritableKeyPath: ExpressionOperable {
    public func asExpression() -> Expression {
        return asProperty().asExpression()
    }
}

extension WritableKeyPath: PropertyRedirectable {
    public func `as`(_ columnBinding: ColumnBindingRepresentable) -> Property {
        return asProperty().`as`(columnBinding)
    }
}

extension WritableKeyPath: ColumnConvertible {
    public func asColumn() -> Column {
        return asProperty().asColumn()
    }
}

//TODO: Find a new way to get the name of key path
//extension WritableKeyPath {
//    private static func getReferenceTypeName<Root: CodableTable, Value: CodableClassColumn>(fromKeyPath keyPath: WritableKeyPath<Root, Value>) -> String {
//        var root = Root()
//        root[keyPath: keyPath] = Value()
//        
//        let results = Mirror(reflecting: root).children.filter { 
//            guard let value = $0.value as? Value else {
//                return false
//            }
//            return value === root[keyPath: keyPath]
//        }
//        
//        guard results.count == 1 else {
//            switch results.count {
//            case 0:
//                Error.abort("Cannot find the property. You may specify the property name or issue us.")
//            default:
//                Error.abort("Multiple properties found. You may change the \(Root.self).MagicNumber to an unique value")
//            }
//        }
//        return results[0].label!
//    }
//    
//    private static func getOptionalReferenceTypeName<Root: CodableTable, Value: OptionalRepresentable>(fromKeyPath keyPath: WritableKeyPath<Root, Value>) -> String where Value.WrappedType: CodableClassColumn {
//        var root = Root()
//        root[keyPath: keyPath] = Value.WrappedType() as! Value
//        
//        let results = Mirror(reflecting: root).children.filter { 
//            guard let value = $0.value as? Value.WrappedType else {
//                return false
//            }
//            return value === root[keyPath: keyPath].wrapped
//        }
//        
//        guard results.count == 1 else {
//            switch results.count {
//            case 0:
//                Error.abort("Cannot find the property. You may specify the property name or issue us.")
//            default:
//                Error.abort("Multiple properties found. You may change the \(Root.self).MagicNumber to an unique value")
//            }
//        }
//        
//        return results[0].label!
//    }
//    
//    private static func getValueTypeName<Root: CodableTable, Value>(fromKeyPath keyPath: WritableKeyPath<Root, Value>) -> String {
//        guard !(Value.self is AnyClass) else {
//            Error.abort("")
//        }
//        
//        var root = Root()
//        
//        let size = MemoryLayout<Value>.size
//        memset(&root[keyPath: keyPath], Root.magicNumber, size)
//        
//        let results = Mirror(reflecting: root).children.filter { 
//            guard var value = $0.value as? Value else {
//                return false
//            }
//            return memcmp(&value, &root[keyPath: keyPath], size) == 0
//        }
//        
//        guard results.count == 1 else {
//            switch results.count {
//            case 0:
//                Error.abort("Cannot find the property. You may specify the property name or issue us.")
//            default:
//                Error.abort("Multiple properties found. You may change the \(Root.self).MagicNumber to an unique value")
//            }
//        }
//        
//        return results[0].label!
//    }
//}
//
//extension WritableKeyPath where Root: CodableTable, Value: CodableClassColumn {
//    var name: String {
//        return WritableKeyPath.getReferenceTypeName(fromKeyPath: self)
//    }
//}
//
//extension WritableKeyPath where Root: CodableTable, Value: OptionalRepresentable, Value.WrappedType: CodableClassColumn {
//    var name: String {
//        return WritableKeyPath.getOptionalReferenceTypeName(fromKeyPath: self)
//    }
//}
//
//extension WritableKeyPath where Root: CodableTable, Value: CodableEnumColumn {
//    var name: String {
//        return WritableKeyPath.getValueTypeName(fromKeyPath: self)
//    }
//}
//
//extension WritableKeyPath where Root: CodableTable, Value: OptionalRepresentable, Value.WrappedType: CodableEnumColumn {
//    var name: String {
//        return WritableKeyPath.getValueTypeName(fromKeyPath: self)
//    }
//}
//
//extension WritableKeyPath where Root: CodableTable, Value: CodableStructColumn {
//    var name: String {
//        return WritableKeyPath.getValueTypeName(fromKeyPath: self)
//    }
//}
//
//extension WritableKeyPath where Root: CodableTable, Value: OptionalRepresentable, Value.WrappedType: CodableStructColumn {
//    var name: String {
//        return WritableKeyPath.getValueTypeName(fromKeyPath: self)
//    }
//}

