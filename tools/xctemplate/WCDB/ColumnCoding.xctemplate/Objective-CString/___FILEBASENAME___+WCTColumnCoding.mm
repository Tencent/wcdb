//___FILEHEADER___

#import "___VARIABLE_productName___.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface ___VARIABLE_productName___ (WCTColumnCoding) <WCTColumnCoding>
@end

@implementation ___VARIABLE_productName___ (WCTColumnCoding)

+ (instancetype)unarchiveWithWCTValue:(NSString *)value
{
    return /* <#Unarchive ___VARIABLE_productName___ From NSString *#> */;
}

- (NSString *)archivedWCTValue
{
    return /* <#Archive NSString * To ___VARIABLE_productName___#> */;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeString;
}

@end
