//
//  WCTSampleTransaction.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleTransaction+WCTTableCoding.h"
#import "WCTSampleTransaction.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleTransaction

WCDB_IMPLEMENTATION(WCTSampleTransaction)
WCDB_SYNTHESIZE(WCTSampleTransaction, intValue)

@end
