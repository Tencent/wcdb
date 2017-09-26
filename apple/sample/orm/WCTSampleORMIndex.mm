//
//  WCTSampleORMIndex.mm
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleORMIndex.h"
#import "WCTSampleORMIndex+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleORMIndex

WCDB_IMPLEMENTATION(WCTSampleORMIndex)
WCDB_SYNTHESIZE(WCTSampleORMIndex, indexProperty)
WCDB_SYNTHESIZE(WCTSampleORMIndex, uniqueIndexProperty)
WCDB_SYNTHESIZE(WCTSampleORMIndex, multiIndexPart1)
WCDB_SYNTHESIZE(WCTSampleORMIndex, multiIndexPart2)

//Index Name = Table Name + Index Subfix Name
WCDB_INDEX(WCTSampleORMIndex, "_indexSubfix", indexProperty)

WCDB_UNIQUE_INDEX(WCTSampleORMIndex, "_uniqueIndexSubfix", uniqueIndexProperty)

//Use the same index subfix name to combine multiple properties into a multi indexes constraint
WCDB_INDEX(WCTSampleORMIndex, "_multiIndexSubfix", multiIndexPart1)
WCDB_INDEX(WCTSampleORMIndex, "_multiIndexSubfix", multiIndexPart2)

@end
