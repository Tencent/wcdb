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

#import "sample_file_main.h"
#import "WCTSampleFile+WCTTableCoding.h"
#import "WCTSampleFile.h"

void sample_file_main(NSString *baseDirectory)
{
    NSLog(@"Sample-file Begin");
    NSString *className = NSStringFromClass(WCTSampleFile.class);
    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
    NSString *otherDirectory = [baseDirectory stringByAppendingPathComponent:@"moved"];
    NSString *tableName = className;
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];

    BOOL ret = [database createTableAndIndexesOfName:tableName withClass:WCTSampleFile.class];
    assert(ret);

    //Get file size
    {
        __block WCTError *error = nil;
        __block size_t fileSize;
        [database close:^{
          //you can call [getFilesSizeWithError:] for an unclosed database, but you will an inaccurate result and a warning
          fileSize = [database getFilesSizeWithError:&error];
        }];
        if (error) {
            NSLog(@"Get file size Error %@", error);
        }
    }

    //Move files
    {
        [database close:^{
          WCTError *error = nil;
          BOOL ret = [database moveFilesToDirectory:otherDirectory withError:&error];
          if (!ret) {
              NSLog(@"Move files Error %@", error);
          }
        }];
    }

    //Get paths
    {
        NSArray *paths = [database getPaths];
    }
    NSLog(@"Sample-file End");
}
