//
//  WCTSampleFTSData.m
//  WCDB
//
//  Created by sanhuazhang on 15/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleFTSData.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleFTSData
WCDB_IMPLEMENTATION(WCTSampleFTSData)
WCDB_SYNTHESIZE(WCTSampleFTSData, name)
WCDB_SYNTHESIZE(WCTSampleFTSData, content)

WCDB_VIRTUAL_TABLE_TOKENIZE(WCTSampleFTSData, WCTTokenizerNameApple)
@end
