//___FILEHEADER___

#import "___VARIABLE_productName___.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface ___VARIABLE_productName___ (WCTColumnCoding) <WCTColumnCoding>
@end

@implementation ___VARIABLE_productName___ (WCTColumnCoding)

+ (instancetype)unarchiveWithWCTValue:(NSData *)value
{
    return /* <#Unarchive ___VARIABLE_productName___ From NSData *#> */;
}

- (NSData *)archivedWCTValue
{
    return /* <#Archive NSData * To ___VARIABLE_productName___#> */;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeBinary;
}

@end
