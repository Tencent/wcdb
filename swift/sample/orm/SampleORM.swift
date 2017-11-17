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

import WCDB

enum SampleORMType: Int, EnumColumnCoding {
    case SampleORMType1 = 1
    case SampleORMType2 = 2
}

class SampleORM: WCDB.TableCoding {
    var identifier: Int = 0
    var desc: String = "nil"
    var value: Double = 0
    var timestamp: String? = nil
    var type: SampleORMType? = nil
    
    required init() {}

    //WCDB
    static let objectRelationalMapping: TableBinding = TableBinding(SampleORM.self)
}

//WCDB
extension SampleORM {    
    static func columnBindings() -> [AnyColumnBinding] {
        return [
            ColumnBinding(\SampleORM.identifier, isPrimary: true),
            ColumnBinding(\SampleORM.desc),
            ColumnBinding(\SampleORM.value, defaultTo: 1.0),
            ColumnBinding(\SampleORM.timestamp, defaultTo: .CurrentTimestamp),
            ColumnBinding(\SampleORM.type)]
    }
}
