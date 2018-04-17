//
//  Interface.h
//  WCDB
//
//  Created by sanhuazhang on 2018/4/17.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

#ifndef Interface_h
#define Interface_h

#import <WCDB/Abstract.h>
#import <WCDB/Core.h>

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
#import <WCDB/WCTDatabase+RepairKit.h>
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

#import <WCDB/WCTCoreError.h>
#import <WCDB/WCTError.h>
#import <WCDB/WCTFileError.h>
#import <WCDB/WCTHandleError.h>
#import <WCDB/WCTPerformanceFootprint.h>
#import <WCDB/WCTSQLiteError.h>
#import <WCDB/WCTStatistics.h>

#import <WCDB/WCTDatabase+FTS.h>
#import <WCDB/WCTTokenizer.h>

#import <WCDB/WCTMigrationDatabase.h>
#import <WCDB/WCTMigrationInfo.h>

#endif /* Interface_h */
