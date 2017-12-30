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
import WCDBSwift

func sample_orm_main(baseDirectory: String) {
    print("Sample-ORM Begin")

    func printSchemas<TableDecodableType: TableDecodable>(of type: TableDecodableType.Type) {
        let className = String(describing: type)
        let filename = className
        let tableName = className

        let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(filename).path
        let database = Database(withPath: path)
        database.close(onClosed: {
            try? database.removeFiles()
        })

        do {
            try database.create(table: tableName, of: type)
        } catch let error {
            print("create table error: \(error)")
        }

        do {
            let schemas: [Master] = try database.getObjects(on: Master.Properties.name, Master.Properties.sql,
                                                            fromTable: Master.tableName)
            schemas.forEach({ (table) in
                print("SQL of \(table.name ?? ""): \(table.sql ?? "" )")
            })
        } catch let error {
            print("get schemas error: \(error)")
        }
    }

    printSchemas(of: SampleORM.self)
    printSchemas(of: SampleORMIndex.self)
    printSchemas(of: SampleORMColumnConstraint.self)
    printSchemas(of: SampleORMTableConstraint.self)

    print("Sample-ORM End")
}
