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

#import <WCDB/Interface.h>
#import <WCDB/WCTError+Private.h>

static_assert((int) WCTHandleOperationNotSet == (int) WCDB::HandleError::Operation::NotSet, "");
static_assert((int) WCTHandleOperationPrepare == (int) WCDB::HandleError::Operation::Prepare, "");
static_assert((int) WCTHandleOperationExecute == (int) WCDB::HandleError::Operation::Execute, "");
static_assert((int) WCTHandleOperationOpen == (int) WCDB::HandleError::Operation::Open, "");
static_assert((int) WCTHandleOperationStep == (int) WCDB::HandleError::Operation::Step, "");
static_assert((int) WCTHandleOperationCipher == (int) WCDB::HandleError::Operation::Cipher, "");
static_assert((int) WCTHandleOperationRepair == (int) WCDB::HandleError::Operation::Repair, "");
static_assert((int) WCTHandleOperationBackup == (int) WCDB::HandleError::Operation::Backup, "");

@implementation WCTHandleError

- (instancetype)initWithWCDBError:(const WCDB::Error *)error
{
    if (self = [super initWithWCDBError:error]) {
        assert(error->getHashedTypeid() == typeid(WCDB::HandleError).hash_code());
        const WCDB::HandleError *handleError = static_cast<const WCDB::HandleError *>(error);
        _tag = handleError->tag;
        _extendedCode = handleError->extendedCode;
        _path = @(handleError->path.c_str());
        _statement = handleError->statement;
        _operation = (WCTHandleOperation) handleError->operation;
    }
    return self;
}

- (WCTErrorType)type
{
    return WCTErrorTypeHandle;
}

- (NSString *)description
{
    NSMutableString *desc = [[NSMutableString alloc] initWithString:[super description]];
    [desc appendFormat:@"Tag: %d", _tag];
    [desc appendFormat:@"ExtCode: %d", _extendedCode];
    [desc appendFormat:@"Path: %@", _path];
    [desc appendFormat:@"SQL: %s", _statement.getDescription().c_str()];
    [desc appendFormat:@"Opeartion: %lu", (unsigned long) _operation];
    return desc;
}

@end
