//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import "___FILEBASENAMEASIDENTIFIER___.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface ___FILEBASENAMEASIDENTIFIER___ (WCTColumnCoding) <WCTColumnCoding>
@end

@implementation ___FILEBASENAMEASIDENTIFIER___ (WCTColumnCoding)

+ (instancetype)unarchiveWithWCTValue:(NSNumber *)value
{
    return <#Unarchive ___FILEBASENAMEASIDENTIFIER___ From NSNumber *#>;
}

- (NSNumber *)archivedWCTValue
{
    return <#Archive NSNumber * To ___FILEBASENAMEASIDENTIFIER___ #>;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeInteger32;
}

@end
