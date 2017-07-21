//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import "___FILEBASENAME___+WCTTableCoding.h"

@implementation ___FILEBASENAMEASIDENTIFIER___ (WCTTableCoding)

WCDB_IMPLEMENTATION(___FILEBASENAMEASIDENTIFIER___)
WCDB_SYNTHESIZE(___FILEBASENAMEASIDENTIFIER___, <#property1 #>)
WCDB_SYNTHESIZE_COLUMN(___FILEBASENAMEASIDENTIFIER___, <#property2 #>, "<#column_name_in_database#>")
WCDB_SYNTHESIZE_DEFAULT(___FILEBASENAMEASIDENTIFIER___, <#property3 #>, <#default_value #>)
WCDB_SYNTHESIZE_COLUMN_DEFAULT(___FILEBASENAMEASIDENTIFIER___, <#property4 #>, "<#column_name_in_database#>", <#default_value #>)
WCDB_SYNTHESIZE(___FILEBASENAMEASIDENTIFIER___, <#... #>)

WCDB_PRIMARY_AUTO_INCREMENT(___FILEBASENAMEASIDENTIFIER___, <#primary_property #>)

WCDB_INDEX(___FILEBASENAMEASIDENTIFIER___, "<#index_subfix_name#>", <#index_property #>)

@end
