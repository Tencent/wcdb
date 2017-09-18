//
//  WCTSampleFTSData+WCTTableCoding.h
//  WCDB
//
//  Created by sanhuazhang on 15/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleFTSData.h"
#import <WCDB/WCDB.h>

@interface WCTSampleFTSData (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(name)
WCDB_PROPERTY(content)

@end
