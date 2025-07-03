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

public final class ForeignKey: Identifier<CPPForeignKey> {

    public init() {
        super.init(with: WCDBForeignKeyCreate())
    }

    @discardableResult
    public func references(with table: String) -> ForeignKey {
        WCDBForeignKeyConfigReferencesTable(cppObj, table.cString)
        return self
    }

    @discardableResult
    public func columns(_ columnConvertibleList: [ColumnConvertible]) -> ForeignKey {
        for columnConvertible in columnConvertibleList {
            let column = columnConvertible .asColumn()
            withExtendedLifetime(column) {
                WCDBForeignKeyAddColumn(cppObj, column.cppObj)
            }
        }
        return self
    }

    @discardableResult
    public func columns(_ columnConvertibleList: ColumnConvertible...) -> ForeignKey {
        return self.columns(columnConvertibleList)
    }

    public enum Action {
        case setNull
        case setDefault
        case cascade
        case restrict
        case noAction
    }

    @discardableResult
    public func onDelete(_ action: Action) -> ForeignKey {
        switch action {
        case .setNull:
            WCDBForeignKeyConfigOnDeleteSetNull(cppObj)
        case .setDefault:
            WCDBForeignKeyConfigOnDeleteSetDefault(cppObj)
        case .cascade:
            WCDBForeignKeyConfigOnDeleteCascade(cppObj)
        case .restrict:
            WCDBForeignKeyConfigOnDeleteRestrict(cppObj)
        case .noAction:
            WCDBForeignKeyConfigOnDeleteNoAction(cppObj)
        }
        return self
    }

    @discardableResult
    public func onUpdate(_ action: Action) -> ForeignKey {
        switch action {
        case .setNull:
            WCDBForeignKeyConfigOnUpdateSetNull(cppObj)
        case .setDefault:
            WCDBForeignKeyConfigOnUpdateSetDefault(cppObj)
        case .cascade:
            WCDBForeignKeyConfigOnUpdateCascade(cppObj)
        case .restrict:
            WCDBForeignKeyConfigOnUpdateRestrict(cppObj)
        case .noAction:
            WCDBForeignKeyConfigOnUpdateNoAction(cppObj)
        }
        return self
    }

    public enum MatchType {
        case Simple
        case Full
        case Partial
    }

    @discardableResult
    public func match(with type: MatchType) -> ForeignKey {
        switch type {
        case .Simple:
            WCDBForeignKeyConfigMatch(cppObj, WCDBSyntaxMatchType_Simple)
        case .Full:
            WCDBForeignKeyConfigMatch(cppObj, WCDBSyntaxMatchType_Full)
        case .Partial:
            WCDBForeignKeyConfigMatch(cppObj, WCDBSyntaxMatchType_Partial)
        }
        return self
    }

    public enum Deferrable {
        case deferred
        case immediate
    }

    @discardableResult
    public func deferrable(_ deferrable: Deferrable? = nil) -> ForeignKey {
        if let deferrable = deferrable {
            switch deferrable {
            case .deferred:
                WCDBForeignKeyConfigDeferrableInitiallyDeferred(cppObj)
            case .immediate:
                WCDBForeignKeyConfigDeferrableInitiallyImmediate(cppObj)
            }
        } else {
            WCDBForeignKeyConfigDeferrable(cppObj)
        }
        return self
    }

    @discardableResult
    public func notDeferrable(_ deferrable: Deferrable? = nil) -> ForeignKey {
        if let deferrable = deferrable {
            switch deferrable {
            case .deferred:
                WCDBForeignKeyConfigNotDeferrableInitiallyDeferred(cppObj)
            case .immediate:
                WCDBForeignKeyConfigNotDeferrableInitiallyImmediate(cppObj)
            }
        } else {
            WCDBForeignKeyConfigNotDeferrable(cppObj)
        }
        return self
    }
}
