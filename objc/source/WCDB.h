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

#ifndef _WCDB_WCDB_H
#define _WCDB_WCDB_H

#ifndef __cplusplus
#error Since WCDB is an Objective-C++ framework, for those files in your project that includes WCDB, you should rename their extension `.m` to `.mm`.
#endif

#warning TODO 1. add empty virtual destructor for all non-final classes 2. add final keyword for all final classes and methods 3. add WCDB prefix for all ifndef

#import <WCDB/WINQ.h>

#import <WCDB/NSData+WCTColumnCoding.h>
#import <WCDB/NSDate+WCTColumnCoding.h>
#import <WCDB/NSNull+WCTColumnCoding.h>
#import <WCDB/NSNumber+WCTColumnCoding.h>
#import <WCDB/NSObject+WCTColumnCoding.h>
#import <WCDB/NSString+WCTColumnCoding.h>

#import <WCDB/WCTValue.h>

#import <WCDB/WCTMaster+WCTTableCoding.h>
#import <WCDB/WCTMaster.h>
#import <WCDB/WCTSequence+WCTTableCoding.h>
#import <WCDB/WCTSequence.h>

#import <WCDB/WCTDatabase+ChainCall.h>
#import <WCDB/WCTHandle+ChainCall.h>
#import <WCDB/WCTTable+ChainCall.h>

#import <WCDB/WCTDelete.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTUpdate.h>

#import <WCDB/WCTDatabase+Config.h>
#import <WCDB/WCTDatabase+File.h>
#import <WCDB/WCTDatabase+Handle.h>
#import <WCDB/WCTDatabase+Memory.h>
#import <WCDB/WCTDatabase+Repair.h>
#import <WCDB/WCTDatabase+Statictics.h>
#import <WCDB/WCTDatabase+Transaction.h>

#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTHandle.h>

#import <WCDB/WCTORM.h>

#import <WCDB/WCTDatabase+Transaction.h>
#import <WCDB/WCTHandle+Transaction.h>
#import <WCDB/WCTTable+Transaction.h>
#import <WCDB/WCTTransaction.h>

#import <WCDB/WCTDatabase+Convenient.h>
#import <WCDB/WCTHandle+Convenient.h>
#import <WCDB/WCTTable+Convenient.h>

#import <WCDB/WCTDatabase+Table.h>
#import <WCDB/WCTHandle+Table.h>
#import <WCDB/WCTTable.h>

#import <WCDB/WCTError.h>
#import <WCDB/WCTPerformanceFootprint.h>

#import <WCDB/WCTDatabase+FTS.h>

#endif /* _WCDB_WCDB_H */
