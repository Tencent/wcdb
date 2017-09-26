//
//  WCTSampleConvenient.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleConvenient+WCTTableCoding.h"
#import "WCTSampleConvenient.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleConvenient

WCDB_IMPLEMENTATION(WCTSampleConvenient)
WCDB_SYNTHESIZE(WCTSampleConvenient, intValue)
WCDB_SYNTHESIZE(WCTSampleConvenient, stringValue)

WCDB_PRIMARY_AUTO_INCREMENT(WCTSampleConvenient, intValue)

@end
