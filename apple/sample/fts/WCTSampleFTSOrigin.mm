//
//  WCTSampleFTSOrigin.m
//  WCDB
//
//  Created by sanhuazhang on 15/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleFTSOrigin.h"
#import "WCTSampleFTSOrigin+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleFTSOrigin

WCDB_IMPLEMENTATION(WCTSampleFTSOrigin)
WCDB_SYNTHESIZE(WCTSampleFTSOrigin, localID)
WCDB_SYNTHESIZE(WCTSampleFTSOrigin, name)
WCDB_SYNTHESIZE(WCTSampleFTSOrigin, content)

WCDB_PRIMARY_AUTO_INCREMENT(WCTSampleFTSOrigin, localID)

@end
