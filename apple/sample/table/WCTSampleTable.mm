//
//  WCTSampleTable.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleTable+WCTTableCoding.h"
#import "WCTSampleTable.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleTable

WCDB_IMPLEMENTATION(WCTSampleTable)
WCDB_SYNTHESIZE(WCTSampleTable, intValue)
WCDB_SYNTHESIZE(WCTSampleTable, stringValue)

@end
