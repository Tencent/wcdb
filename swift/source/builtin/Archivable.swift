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

/*
 * Builtin codable implementation for archivable type
 *
 * Array, Dictionary, Set
 * all [NSObject]s, e.g. NSArray/NSMutableArray, NSDictionary/NSMutableDictionary, NSSet/NSMutableSet
 * URL
 * 
 */

public protocol ArchivableColumn: CodableColumn {}

extension ArchivableColumn {
    public typealias FundamentalType = Data
    
    public init?(withTypedValue optionalValue: Data?) {
        guard let value = optionalValue, value.count > 0 else {
            return nil
        }
        guard let unarchived = NSKeyedUnarchiver.unarchiveObject(with: value), let object = unarchived as? Self else {
            return nil
        }
        self = object
    }
    
    public func archivedTypedValue() -> Data? {
        return NSKeyedArchiver.archivedData(withRootObject: self)
    }
}

public typealias ArchivableEnumColumn = CodableEnumColumn & ArchivableColumn
public typealias ArchivableStructColumn = CodableStructColumn & ArchivableColumn
public typealias ArchivableClassColumn = CodableClassColumn & ArchivableColumn

extension Array: ArchivableStructColumn {}
extension Dictionary: ArchivableStructColumn {}
extension Set: ArchivableStructColumn {}

extension NSObject: ArchivableClassColumn {}

extension URL: ArchivableStructColumn {}
