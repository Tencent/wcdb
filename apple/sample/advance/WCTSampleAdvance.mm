//
//  WCTSampleAdvance.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleAdvance+WCTTableCoding.h"
#import "WCTSampleAdvance.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleAdvance

WCDB_IMPLEMENTATION(WCTSampleAdvance)
WCDB_SYNTHESIZE(WCTSampleAdvance, intValue)
WCDB_SYNTHESIZE(WCTSampleAdvance, columnCoding)

WCDB_PRIMARY_ASC_AUTO_INCREMENT(WCTSampleAdvance, intValue)

@end
