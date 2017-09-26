//
//  WCTSampleRepair.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleRepair+WCTTableCoding.h"
#import "WCTSampleRepair.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleRepair

WCDB_IMPLEMENTATION(WCTSampleRepair)
WCDB_SYNTHESIZE(WCTSampleRepair, identifier)
WCDB_SYNTHESIZE(WCTSampleRepair, content)

@end
