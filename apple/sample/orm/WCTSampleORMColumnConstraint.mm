//
//  WCTSampleORMColumnConstraint.mm
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleORMColumnConstraint.h"
#import "WCTSampleORMColumnConstraint+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleORMColumnConstraint

WCDB_IMPLEMENTATION(WCTSampleORMColumnConstraint)
WCDB_SYNTHESIZE(WCTSampleORMColumnConstraint, primaryProperty)
WCDB_SYNTHESIZE(WCTSampleORMColumnConstraint, uniqueProperty)
WCDB_SYNTHESIZE(WCTSampleORMColumnConstraint, notNullProperty)

WCDB_PRIMARY_ASC_AUTO_INCREMENT(WCTSampleORMColumnConstraint, primaryProperty)
WCDB_UNIQUE(WCTSampleORMColumnConstraint, uniqueProperty)
WCDB_NOT_NULL(WCTSampleORMColumnConstraint, notNullProperty)

@end
