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

// Column 
public protocol ColumnCodableBase {
    static var columnType: ColumnType {get}
}

public protocol ColumnEncodable: Encodable, ColumnCodableBase {
    func archivedValue() -> FundamentalValue
}
public extension ColumnEncodable where Self: LiteralValueConvertible {
    func asLiteralValue() -> LiteralValue {
        return LiteralValue(self)
    }
}

public protocol ColumnDecodable: Decodable, ColumnCodableBase {
    init?(with value: FundamentalValue)
}

public typealias ColumnCodable = ColumnCodableBase & ColumnEncodable & ColumnDecodable
