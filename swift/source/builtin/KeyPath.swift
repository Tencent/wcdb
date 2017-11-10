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

extension AnyKeyPath: PropertyOperable {
    public func asProperty() -> Property {
        let cls = type(of: self)
        if cls.rootType is TableCoding.Type {
            let property = (cls.rootType as! TableCoding.Type).property(fromAccessor: self)
            property.columnBinding = self.columnBinding
            return property
        }
        fatalError("\(cls.rootType) is not conform to protocol: TableCoding")
    }
}

extension AnyKeyPath: ColumnBindingRepresentable {
    public var columnBinding: AnyColumnBinding? {
        let cls = type(of: self)
        if cls.rootType is TableCoding.Type {
            return (cls.rootType as! TableCoding.Type).objectRelationalMapping.columnBinding(fromAccessor: self)
        }
        fatalError("\(cls.rootType) is not conform to protocol: TableCoding")
    }
}

//FIXME: refactor
extension WritableKeyPath where Root: TableCoding, Value: OptionalExpressible, Value.WrappedType: ColumnCoding {
    public var name: String {
        var propertyName: String? = nil
        
        var root = Root()
        var property = Value.WrappedType()
        var isSameProperty: (inout Value.WrappedType, inout Value.WrappedType) -> Bool
        if Value.WrappedType.self is AnyClass {
            //TODO: use === ?
            isSameProperty = { unsafeBitCast($0, to: Int.self)==unsafeBitCast($1, to: Int.self) }
        }else {
            let size = MemoryLayout<Value.WrappedType>.size
            guard size > 0 else {
                fatalError("A non-class property with empty memory size is not supported")
            }
            memset(&property, Root.magicNumber, size)
            isSameProperty = { memcmp(&$0, &$1, size)==0 }
        }
        
        let o: Value.WrappedType? = property
        root[keyPath: self] = o as! Value 
        
        Mirror(reflecting: root).children.forEach { (child) in
            guard var value = child.value as? Value.WrappedType else {
                return
            }
            guard isSameProperty(&value, &property) else {
                return
            }
            guard propertyName == nil else {
                fatalError("Multiple properties found. You may change the \(Root.self).MagicNumber to an unique value")
            }
            propertyName = child.label
        }
        assert(propertyName != nil)
        return propertyName!
    }
}

extension WritableKeyPath where Root: TableCoding, Value: ColumnCoding {
    public var name: String {
        var propertyName: String? = nil
        
        var root = Root()
        root[keyPath: self] = Value()
        var isSameProperty: (inout Value, inout Value) -> Bool
        if Value.self is AnyClass {
            //TODO: use === ?
            isSameProperty = { unsafeBitCast($0, to: Int.self)==unsafeBitCast($1, to: Int.self) }
        }else {
            let size = MemoryLayout<Value>.size
            guard size > 0 else {
                fatalError("A non-class property with empty memory size is not supported")
            }
            memset(&root[keyPath: self], Root.magicNumber, size)
            isSameProperty = { memcmp(&$0, &$1, size)==0 }
        }
        
        Mirror(reflecting: root).children.forEach { (child) in
            guard var value = child.value as? Value else {
                return
            }
            guard isSameProperty(&value, &root[keyPath: self]) else {
                return 
            }
            guard propertyName == nil else {
                fatalError("Multiple properties found. You may change the \(Root.self).MagicNumber to an unique value")
            }
            propertyName = child.label
        }
        assert(propertyName != nil)
        return propertyName!
    }
}
