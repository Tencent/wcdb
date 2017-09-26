//
//  WCTSampleORMColumnConstraint+WCTTableCoding.h
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleORMColumnConstraint.h"
#import <WCDB/WCDB.h>

@interface WCTSampleORMColumnConstraint (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(primaryProperty)
WCDB_PROPERTY(uniqueProperty)
WCDB_PROPERTY(notNullProperty)

@end
