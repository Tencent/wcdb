/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "WINQTestCase.h"

@implementation WINQTestCase

+ (std::list<WCDB::Syntax::Identifier::Type>)getTypes:(WCDB::SQL&)sql
{
    std::list<WCDB::Syntax::Identifier::Type> types;
    sql.iterate([&types](WCDB::Syntax::Identifier& identifier, bool& stop) {
        types.push_back(identifier.getType());
    });
    return types;
}

+ (NSString*)getTypesDescription:(const std::list<WCDB::Syntax::Identifier::Type>&)types
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

+ (NSString*)hintForTypes:(const std::list<WCDB::Syntax::Identifier::Type>&)types expecting:(const std::list<WCDB::Syntax::Identifier::Type>&)expected
{
    return [self hint:[self getTypesDescription:types] expecting:[self getTypesDescription:expected]];
}

@end
