//
//  WINQAssertion.m
//  WINQ
//
//  Created by sanhuazhang(张三华) on 2019/1/7.
//  Copyright © 2019 sanhuazhang. All rights reserved.
//

#import "WINQAssertion.h"

namespace WCDB {

std::list<WCDB::Syntax::Identifier::Type> getTypesOfSQL(WCDB::SQL& sql)
{
    std::list<WCDB::Syntax::Identifier::Type> types;
    sql.iterate([&types](WCDB::Syntax::Identifier& identifier, bool& stop) {
        types.push_back(identifier.getType());
    });
    return types;
}

NSString* getDescriptionOfTypes(const std::list<WCDB::Syntax::Identifier::Type>& types)
{
    NSMutableString* description = [NSMutableString string];
    BOOL comma = NO;
    for (const auto& type : types) {
        if (comma) {
            [description appendString:@", "];
        } else {
            comma = true;
        }
        [description appendString:@(WCDB::Enum::description(type))];
    }
    return description;
}

}
