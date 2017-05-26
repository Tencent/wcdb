//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface ___FILEBASENAMEASIDENTIFIER___(WCDB) <WCTColumnCoding>
@end

@implementation ___FILEBASENAMEASIDENTIFIER___(WCDB)

+ (instancetype)unarchiveWithWCTValue:(NSString*)value
{
    return <#Unarchive ___FILEBASENAMEASIDENTIFIER___ From NSString*#>;
}

- (NSString*)archivedWCTValue
{
    return <#Archive NSString* To ___FILEBASENAMEASIDENTIFIER___#>;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeString;
}

@end

