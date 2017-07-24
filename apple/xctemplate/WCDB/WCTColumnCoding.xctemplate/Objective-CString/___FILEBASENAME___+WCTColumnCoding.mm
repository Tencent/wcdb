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

+ (instancetype)unarchiveWithWCTValue:(NSString *)value
{
    return <#Unarchive ___FILEBASENAMEASIDENTIFIER___ From NSString *#>;
}

- (NSString *)archivedWCTValue
{
    return <#Archive NSString * To ___FILEBASENAMEASIDENTIFIER___ #>;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeString;
}

@end
