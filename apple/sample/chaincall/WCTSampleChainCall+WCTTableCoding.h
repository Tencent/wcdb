//
//  WCTSampleChainCall+WCTTableCoding.h
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleChainCall.h"
#import <WCDB/WCDB.h>

@interface WCTSampleChainCall (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(intValue)
WCDB_PROPERTY(stringValue)

@end
