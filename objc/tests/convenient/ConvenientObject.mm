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

#import "ConvenientObject.h"
#import "ConvenientObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation ConvenientObject

WCDB_IMPLEMENTATION(ConvenientObject)
WCDB_SYNTHESIZE(ConvenientObject, identifier)
WCDB_SYNTHESIZE(ConvenientObject, content)

WCDB_PRIMARY_ASC_AUTO_INCREMENT(ConvenientObject, identifier)

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    ConvenientObject* other = (ConvenientObject*) object;
    if (self.identifier != other.identifier) {
        return NO;
    }
    if (self.content != nil) {
        return [other.content isEqualToString:self.content];
    } else {
        return other.content == nil;
    }
}

@end
