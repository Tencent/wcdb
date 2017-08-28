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

#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTError.h>
#import <WCDB/WCTInterface+Core.h>
#import <WCDB/WCTStatement+Private.h>

@implementation WCTInterface (Core)

- (BOOL)exec:(const WCDB::Statement &)statement
{
    WCDB::Error innerError;
    return _core->exec(statement, innerError);
}

- (WCTStatement *)prepare:(const WCDB::Statement &)statement
{
    return [self prepare:statement withError:nil];
}

- (WCTStatement *)prepare:(const WCDB::Statement &)statement withError:(WCTError *__autoreleasing *)error
{
    WCDB::Error innerError;
    WCDB::RecyclableStatement statementHandle = _core->prepare(statement, innerError);
    if (statementHandle) {
        return [[WCTStatement alloc] initWithCore:_core andStatementHandle:statementHandle];
    }
    if (error) {
        *error = [WCTError errorWithWCDBError:innerError];
    }
    return nil;
}

@end
