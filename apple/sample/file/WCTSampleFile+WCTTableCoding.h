//
//  WCTSampleFile+WCTTableCoding.h
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleFile.h"
#import <WCDB/WCDB.h>

@interface WCTSampleFile (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(intValue)

@end
