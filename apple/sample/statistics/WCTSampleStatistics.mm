//
//  WCTSampleStatistics.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleStatistics+WCTTableCoding.h"
#import "WCTSampleStatistics.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleStatistics

WCDB_IMPLEMENTATION(WCTSampleStatistics)
WCDB_SYNTHESIZE(WCTSampleStatistics, intValue)

WCDB_UNIQUE(WCTSampleStatistics, intValue)

@end
