//
//  WCTSampleRepair+WCTTableCoding.h
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleRepair.h"
#import <WCDB/WCDB.h>

@interface WCTSampleRepair (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(identifier)
WCDB_PROPERTY(content)

@end
