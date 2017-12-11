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

#import "sample_advance_main.h"
#import "sample_chaincall_main.h"
#import "sample_convenient_main.h"
#import "sample_core_main.h"
#import "sample_file_main.h"
#import "sample_fts_main.h"
#import "sample_orm_main.h"
#import "sample_repair_main.h"
#import "sample_statistics_main.h"
#import "sample_table_main.h"
#import "sample_transaction_main.h"
#import <Foundation/Foundation.h>

int main(int argc, const char *argv[])
{
    NSString *machPath = [NSString stringWithUTF8String:argv[0]];
    NSString *baseDirectory = [[machPath stringByDeletingLastPathComponent] stringByAppendingPathComponent:@"SampleDB"];
    NSLog(@"Base Directory: %@", baseDirectory);
    sample_orm_main(baseDirectory);
    sample_convenient_main(baseDirectory);
    sample_chaincall_main(baseDirectory);
    sample_file_main(baseDirectory);
    sample_table_main(baseDirectory);
    sample_transaction_main(baseDirectory);
    sample_advance_main(baseDirectory);
    sample_repair_main(baseDirectory);
    sample_core_main(baseDirectory);
    sample_statistics_main(baseDirectory);
    sample_fts_main(baseDirectory);
    return 0;
}
