//
//  WCTSampleTransaction+WCTTableCoding.h
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleTransaction.h"
#import <WCDB/WCDB.h>

@interface WCTSampleTransaction (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(intValue)

@end
