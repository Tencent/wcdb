//
// Created by qiuwenchen on 2022/6/11.
//

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
import WCDB_Private

public final class QualifiedTable: Identifier<CPPQualifiedTable> {
    public init(with name: String) {
        super.init(with: WCDBQualifiedTableCreateWithTable(name.cString))
    }

    @discardableResult
    public func of(schema schemaConvertible: SchemaConvertible) -> QualifiedTable {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBQualifiedTableConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func `as`(_ alias: String) -> QualifiedTable {
        WCDBQualifiedTableConfigAliasName(cppObj, alias.cString)
        return self
    }

    @discardableResult
    public func indexed(by indexName: String) -> QualifiedTable {
        WCDBQualifiedTableConfigIndexName(cppObj, indexName.cString)
        return self
    }

    @discardableResult
    public func notIndexed() -> QualifiedTable {
        WCDBQualifiedTableConfigNoIndexed(cppObj)
        return self
    }
}

extension QualifiedTable: QualifiedTableConvertible {
    public func asQualifiedTable() -> QualifiedTable {
        return self
    }
}

extension String: QualifiedTableConvertible {
    public func asQualifiedTable() -> QualifiedTable {
        return QualifiedTable(with: self)
    }
}
