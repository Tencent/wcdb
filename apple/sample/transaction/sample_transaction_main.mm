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

#import "sample_transaction_main.h"
#import "WCTSampleTransaction+WCTTableCoding.h"
#import "WCTSampleTransaction.h"

void sample_transaction_main(NSString *baseDirectory)
{
    NSLog(@"Sample-transaction Begin");
    NSString *className = NSStringFromClass(WCTSampleTransaction.class);
    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
    NSString *tableName = className;
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];

    //Run auto inner create table/indexes transaction
    {
        //[createTableAndIndexesOfName:withClass:] interface will automatically run an inner transaction if it's not already in a transaction
        BOOL ret = [database createTableAndIndexesOfName:tableName withClass:WCTSampleTransaction.class];
    }

    //Run blocked transaction
    {
        BOOL committed = [database runTransaction:^BOOL {
          WCTSampleTransaction *object = [[WCTSampleTransaction alloc] init];
          BOOL ret = [database insertObject:object
                                       into:tableName];
          //return YES to do a commit and return NO to do a rollback
          if (ret) {
              return YES;
          }
          return NO;
        }
            event:^(WCTTransactionEvent event) {
              NSLog(@"Event %d", event);
            }];
    }

    //Run threaded transaction
    {
        //[beginTransaction], [commitTransaction], [rollbackTransaction] and all interfaces inside this transaction should run in same thread
        BOOL ret = [database beginTransaction];
        WCTSampleTransaction *object = [[WCTSampleTransaction alloc] init];
        ret = [database insertObject:object
                                into:tableName];
        if (ret) {
            ret = [database commitTransaction];
        } else {
            ret = [database rollbackTransaction];
        }
    }

    //Transaction using WCTTransaction
    {
        //You can do a transaction in different threads using WCTTransaction.
        //But it's better to run serially, or an inner thread mutex will guarantee this.
        WCTTransaction *transaction = [database getTransaction];
        BOOL ret = [transaction begin];
        dispatch_async(dispatch_queue_create("other thread", DISPATCH_QUEUE_SERIAL), ^{
          WCTSampleTransaction *object = [[WCTSampleTransaction alloc] init];
          BOOL ret = [transaction insertObject:object
                                          into:tableName];
          if (ret) {
              [transaction commit];
          } else {
              [transaction rollback];
          }
        });
    }

    //Run auto inner batch insert transaction
    {
        //insertion will automatically run an inner transaction if it's not already in a transaction and objects is more than 1 objects
        NSMutableArray *objects = [[NSMutableArray alloc] init];
        WCTSampleTransaction *object1 = [[WCTSampleTransaction alloc] init];
        [objects addObject:object1];
        WCTSampleTransaction *object2 = [[WCTSampleTransaction alloc] init];
        [objects addObject:object2];
        BOOL ret = [database insertObjects:objects
                                      into:tableName];
    }
    NSLog(@"Sample-transaction End");
}
