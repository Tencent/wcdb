//
// Created by sanhuazhang on 2019/06/12
//

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

#import "WCTFoundation.h"

@implementation NSString (WCDB)

+ (NSString*)stringWithView:(const WCDB::UnsafeStringView&)string
{
    return [[NSString alloc] initWithBytes:string.data() length:string.length() encoding:NSUTF8StringEncoding];
}

- (NSString*)wcdb_stringByStandardizingPath
{
    NSString* path = [self stringByStandardizingPath];
#ifdef __APPLE__
    /*
     /var is the symlink to /private/var.
     In most cases, -[NSString stringByStandardizingPath] will return the path with the /var prefix,
     while in a small number of cases it will return the path with the /private/var prefix.
     In order to avoid the inconsistency of the path of the same file, remove the /private prefix of path here
     */
    NSRange match = [path rangeOfString:@"/private"];
    if (match.location == 0) {
        path = [path stringByReplacingCharactersInRange:match withString:@""];
    }
#endif
    return path;
}

@end
