//___FILEHEADER___

#import "___VARIABLE_productName___+WCTTableCoding.h"
#import "___VARIABLE_productName___.h"
#import <WCDB/WCDB.h>

@implementation ___VARIABLE_productName___

WCDB_IMPLEMENTATION(___VARIABLE_productName___)
WCDB_SYNTHESIZE(___VARIABLE_productName___, <#property1#>)
WCDB_SYNTHESIZE(___VARIABLE_productName___, <#property2#>)
WCDB_SYNTHESIZE(___VARIABLE_productName___, <#property3#>)
WCDB_SYNTHESIZE(___VARIABLE_productName___, <#property4#>)


WCDB_SYNTHESIZE_COLUMN(___VARIABLE_productName___, <#property5#>, "<#column name#>") //custom column name
WCDB_SYNTHESIZE_DEFAULT(___VARIABLE_productName___, <#property6#>, 1.0f) // for date you can use WCTDefaultTypeCurrentTimestamp

WCDB_PRIMARY(___VARIABLE_productName___, <#property1#>)
WCDB_PRIMARY_ASC_AUTO_INCREMENT(___VARIABLE_productName___, <#property2#>)
WCDB_UNIQUE(___VARIABLE_productName___, <#property3#>)
WCDB_NOT_NULL(___VARIABLE_productName___, <#property4#>)
@end
