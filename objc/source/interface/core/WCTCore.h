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

#import <Foundation/Foundation.h>
#import <WCDB/WCTDeclare.h>

/**
 Thread-safe
 */
@interface WCTCore : NSObject

/**
 @brief The tag of the database. 
        You should set it on a WCTDatabase and can get it from all kind of WCTCore objects, including WCTDatabase, WCTTable, WCTransaction, WCTSelect, WCTRowSelect, WCTMultiSelect, WCTInsert, WCTDelete, WCTUpdate and so on. 
        Note that WCTCore objects with same path share this tag, even they are not the same object.
 
     WCTDatabase* database1 = [[WCTDatabase alloc] initWithPath:path];
     WCTDatabase* database2 = [[WCTDatabase alloc] initWithPath:path];
     database1.tag = 1;
     NSLog(@"%d", database2.tag);//print 1

 @return Default to 0
 */
- (WCTTag)tag;

/**
 @brief The path of the database

 @return path
 */
- (NSString *)path;

@end
