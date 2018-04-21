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

static_assert((int) WCTFileOperationNotSet == (int) WCDB::FileError::Operation::NotSet, "");
static_assert((int) WCTFileOperationLstat == (int) WCDB::FileError::Operation::Lstat, "");
static_assert((int) WCTFileOperationAccess == (int) WCDB::FileError::Operation::Access, "");
static_assert((int) WCTFileOperationLink == (int) WCDB::FileError::Operation::Link, "");
static_assert((int) WCTFileOperationUnlink == (int) WCDB::FileError::Operation::Unlink, "");
static_assert((int) WCTFileOperationRemove == (int) WCDB::FileError::Operation::Remove, "");
static_assert((int) WCTFileOperationMkdir == (int) WCDB::FileError::Operation::Mkdir, "");

@implementation WCTFileError

- (instancetype)initWithFileError:(const WCDB::FileError &)fileError
{
    if (self = [super initWithError:fileError]) {
        _operation = (WCTFileOperation) fileError.operation;
        _path = [NSString stringWithCppString:fileError.path];
    }
    return self;
}

- (WCTErrorType)type
{
    return WCTErrorTypeFile;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"%@, Op: %lu, Path: %@", [super description], (unsigned long) _operation, _path];
}

@end
