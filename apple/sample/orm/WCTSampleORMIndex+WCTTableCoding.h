//
//  WCTSampleORMIndex+WCTTableCoding.h
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleORMIndex.h"
#import <WCDB/WCDB.h>

@interface WCTSampleORMIndex (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(indexProperty)
WCDB_PROPERTY(uniqueIndexProperty)
WCDB_PROPERTY(multiIndexPart1)
WCDB_PROPERTY(multiIndexPart2)

@end
