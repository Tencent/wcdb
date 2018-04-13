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

#import "TestCase.h"

//database1.table1 -> migrated.table1
//database2.table2 -> migrated.table2
//migrated.table3 -> migrated.migratedTable
@interface MultipleMigrationTestCase : TestCase {
    int _count;
    Class _cls;

    NSString *_path1;
    WCTDatabase *_database1;
    NSString *_table1;
    NSArray<TestCaseObject *> *_preInsertObjects1;

    NSString *_path2;
    WCTDatabase *_database2;
    NSString *_table2;
    NSArray<TestCaseObject *> *_preInsertObjects2;

    NSString *_table3;
    NSArray<TestCaseObject *> *_preInsertObjects3;

    WCTMigrationDatabase *_migrated;
    NSString *_migratedTable;
    NSString *_migratedPath;

    NSArray<WCTMigrationInfo *> *_infos;
}

- (NSString *)schemaNameForPath:(NSString *)path;

@end
