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

- (instancetype)initWithWCDBError:(const WCDB::Error *)error
{
    if (self = [super initWithWCDBError:error]) {
        assert(error->getHashedTypeid() == typeid(WCDB::FileError).hash_code());
        const WCDB::FileError *fileError = static_cast<const WCDB::FileError *>(error);
        _operation = (WCTFileOperation) fileError->operation;
        _path = @(fileError->path.c_str());
    }
    return self;
}

- (WCTErrorType)type
{
    return WCTErrorTypeFile;
}

- (NSString *)description
{
    NSMutableString *desc = [[NSMutableString alloc] initWithString:[super description]];
    [desc appendFormat:@"Tag: %lu", (unsigned long) _operation];
    [desc appendFormat:@"Path: %@", _path];
    return desc;
}

@end
