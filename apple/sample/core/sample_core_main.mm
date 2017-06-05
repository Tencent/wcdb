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

#import "sample_core_main.h"
#import <WCDB/WCDB.h>

void sample_core_main(NSString *baseDirectory)
{
    NSLog(@"Sample-core Begin");
    NSString *path = [baseDirectory stringByAppendingPathComponent:@"core"];
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];

    //set config
    [database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> handle, WCDB::Error &error) {
      return handle->exec(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete, YES));
    }
                forName:@"demo"
              withOrder:1];

    //run unwrapped SQL
    [database exec:WCDB::StatementPragma().pragma(WCDB::Pragma::CaseSensitiveLike, true)];

    //get value from unwraaped SQL
    WCTStatement *statement = [database prepare:WCDB::StatementPragma().pragma(WCDB::Pragma::CacheSize)];
    if (statement && statement.step) {
        NSLog(@"Cache size %@", [statement getValueAtIndex:0]);
    }
    NSLog(@"Sample-core End");
}
