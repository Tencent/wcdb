//
//  WCTSampleColumnCoding+WCTColumnCoding.mm
//  Sample
//
//  Created by sanhuazhang on 26/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import "WCTSampleColumnCoding.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface WCTSampleColumnCoding (WCTColumnCoding) <WCTColumnCoding>
@end

@implementation WCTSampleColumnCoding (WCTColumnCoding)

+ (instancetype)unarchiveWithWCTValue:(NSNumber *)value
{
    return value ? [[self alloc] initWithFloatValue:value.floatValue] : nil;
}

- (NSNumber *)archivedWCTValue
{
    return [NSNumber numberWithFloat:self.floatValue];
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeDouble;
}

@end
