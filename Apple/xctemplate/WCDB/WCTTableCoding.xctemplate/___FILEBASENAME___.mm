//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import "___FILEBASENAME___.h"

@implementation ___FILEBASENAMEASIDENTIFIER___

/*
@synthesize primary;
@synthesize valueWithIndex;
@synthesize valueWithSpecifiedColumnName;
@synthesize valueWithDefaultValue;
@synthesize value1WithMultiIndex;
@synthesize value2WithMultiIndex;
*/

//The order of the definitions is the order of the fields in the database
WCDB_IMPLEMENTATION(___FILEBASENAMEASIDENTIFIER___)
/*
//Primary Key
WCDB_SYNTHESIZE(TestTemplate, primary)
//Property With Index
WCDB_SYNTHESIZE(TestTemplate, valueWithIndex)
//Propery With Specified Column Name
WCDB_SYNTHESIZE_COLUMN(TestTemplate, valueWithSpecifiedColumnName, "column_name_in_database")
//Propery With Default Value
WCDB_SYNTHESIZE_DEFAULT(TestTemplate, valueWithDefaultValue, @"default_string_for_database")
//Properies With Multi-Index
WCDB_SYNTHESIZE(TestTemplate, value1WithMultiIndex)
WCDB_SYNTHESIZE(TestTemplate, value2WithMultiIndex)

//Primary Key
WCDB_PRIMARY_ASC_AUTO_INCREMENT(TestTemplate, primary)
//Index
WCDB_INDEX_DESC(TestTemplate, "index_subfix_name", valueWithIndex)
//Multi-Indexes
WCDB_INDEX_DESC(TestTemplate, "mutil_indexes_shared_same_name", value1WithMultiIndex);
WCDB_INDEX_DESC(TestTemplate, "mutil_indexes_shared_same_name", value2WithMultiIndex);
*/

@end
