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

#ifndef WCDB_h
#define WCDB_h

#ifndef __cplusplus
#error Since WCDB is an Objective-C++ framework, for those files in your project that includes WCDB, you should rename their extension .m to .mm.
#endif

#import <WCDB/abstract.h>

#import <WCDB/WCTMaster+WCTTableCoding.h>
#import <WCDB/WCTMaster.h>
#import <WCDB/WCTSequence+WCTTableCoding.h>
#import <WCDB/WCTSequence.h>

#import <WCDB/WCTDelete.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTInterface+ChainCall.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTTable+ChainCall.h>
#import <WCDB/WCTUpdate.h>

#import <WCDB/WCTDatabase+Core.h>
#import <WCDB/WCTInterface+Core.h>

#import <WCDB/WCTDatabase+Database.h>
#import <WCDB/WCTDatabase+FTS.h>
#import <WCDB/WCTDatabase+File.h>
#import <WCDB/WCTDatabase+RepairKit.h>

#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTStatement.h>
#import <WCDB/WCTTable.h>

#import <WCDB/WCTAnyProperty.h>
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>

#import <WCDB/WCTInterface+Convenient.h>
#import <WCDB/WCTTable+Convenient.h>

#import <WCDB/WCTDatabase+Table.h>
#import <WCDB/WCTTransaction+Table.h>

#import <WCDB/WCTDatabase+Transaction.h>
#import <WCDB/WCTTransaction.h>

#import <WCDB/WCTChainCall+Statistics.h>
#import <WCDB/WCTDatabase+Statistics.h>
#import <WCDB/WCTError.h>
#import <WCDB/WCTStatistics.h>
#import <WCDB/WCTTransaction+Statistics.h>

#import <WCDB/WCTTokenizer+Apple.h>
#import <WCDB/WCTTokenizer+WCDB.h>

#ifndef WCDB_OMIT_DEPRECATED

#import <WCDB/WCTCompatible.h>
#import <WCDB/WCTDatabase+Compatible.h>
#import <WCDB/WCTStatement+Compatible.h>
#import <WCDB/WCTStatistics+Compatible.h>
#import <WCDB/WCTTransaction+Compatible.h>

#endif //WCDB_OMIT_DEPRECATED

#endif /* WCDB_h */
