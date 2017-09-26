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

#import "sample_orm_main.h"
#import "WCTSampleORM+WCTTableCoding.h"
#import "WCTSampleORM.h"
#import "WCTSampleORMColumnConstraint+WCTTableCoding.h"
#import "WCTSampleORMColumnConstraint.h"
#import "WCTSampleORMIndex+WCTTableCoding.h"
#import "WCTSampleORMIndex.h"
#import "WCTSampleORMTableConstraint+WCTTableCoding.h"
#import "WCTSampleORMTableConstraint.h"

void sample_orm_main(NSString *baseDirectory)
{
    NSLog(@"Sample-ORM Begin");
    NSArray *clses = @[ WCTSampleORM.class,
                        WCTSampleORMColumnConstraint.class,
                        WCTSampleORMTableConstraint.class,
                        WCTSampleORMIndex.class ];
    for (Class cls in clses) {
        NSString *classString = NSStringFromClass(cls);
        NSString *filename = classString;
        NSString *tableName = classString;

        NSString *path = [baseDirectory stringByAppendingPathComponent:filename];
        WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
        [database close:^{
          [database removeFilesWithError:nil];
        }];

        BOOL ret = [database createTableAndIndexesOfName:tableName withClass:cls];
        assert(ret);

        NSArray *schemas = [database getAllObjectsOnResults:{WCTMaster.name, WCTMaster.sql} fromTable:WCTMaster.TableName];
        for (WCTMaster *table : schemas) {
            NSLog(@"SQL Of %@: %@", table.name, table.sql);
        }
    }
    NSLog(@"Sample-ORM End");
}
