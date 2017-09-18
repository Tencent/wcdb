//
//  WCTSampleFTSOrigin+WCTTableCoding.h
//  WCDB
//
//  Created by sanhuazhang on 15/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleFTSOrigin.h"
#import <WCDB/WCDB.h>

@interface WCTSampleFTSOrigin (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(localID)
WCDB_PROPERTY(name)
WCDB_PROPERTY(content)

@end
