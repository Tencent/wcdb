//
//  WCTSampleChainCall.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleChainCall+WCTTableCoding.h"
#import "WCTSampleChainCall.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleChainCall

WCDB_IMPLEMENTATION(WCTSampleChainCall)
WCDB_SYNTHESIZE(WCTSampleChainCall, intValue)
WCDB_SYNTHESIZE(WCTSampleChainCall, stringValue)

@end
