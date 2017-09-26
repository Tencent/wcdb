//
//  WCTSampleORMTableConstraint.mm
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleORMTableConstraint.h"
#import "WCTSampleORMTableConstraint+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleORMTableConstraint

WCDB_IMPLEMENTATION(WCTSampleORMTableConstraint)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, primaryKeyPart1)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, primaryKeyPart2)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, uniqueKeyPart1)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, uniqueKeyPart2)

//Use the same constraint name to combine multiple properties into a multi primary key constraint
WCDB_MULTI_PRIMARY_DESC(WCTSampleORMTableConstraint, "MultiPrimaryConstraint", primaryKeyPart1)
WCDB_MULTI_PRIMARY(WCTSampleORMTableConstraint, "MultiPrimaryConstraint", primaryKeyPart2)

//Use the same constraint name to combine multiple properties into a multi unique constraint
WCDB_MULTI_UNIQUE(WCTSampleORMTableConstraint, "MultiUniqueConstraint", uniqueKeyPart1)
WCDB_MULTI_UNIQUE_ASC(WCTSampleORMTableConstraint, "MultiUniqueConstraint", uniqueKeyPart2)

@end
