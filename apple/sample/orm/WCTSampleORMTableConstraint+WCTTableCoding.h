//
//  WCTSampleORMTableConstraint+WCTTableCoding.h
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleORMTableConstraint.h"
#import <WCDB/WCDB.h>

@interface WCTSampleORMTableConstraint (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(primaryKeyPart1)
WCDB_PROPERTY(primaryKeyPart2)
WCDB_PROPERTY(uniqueKeyPart1)
WCDB_PROPERTY(uniqueKeyPart2)

@end
