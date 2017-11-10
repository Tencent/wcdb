//___FILEHEADER___

#import "___VARIABLE_productName___.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface ___VARIABLE_productName___ (WCTColumnCoding) <WCTColumnCoding>
@end

@implementation ___VARIABLE_productName___ (WCTColumnCoding)

+ (instancetype)unarchiveWithWCTValue:(NSNumber *)value
{
    return /* <#Unarchive ___VARIABLE_productName___ From NSNumber *#> */;
}

- (NSNumber *)archivedWCTValue
{
    return /* <#Archive NSNumber * To ___VARIABLE_productName___#> */;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeInteger64;
}

@end
