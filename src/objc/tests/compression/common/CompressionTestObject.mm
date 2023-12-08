//
// Created by qiuwenchen on 2023/12/3.
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

#import "CompressionTestObject.h"
#import <Foundation/Foundation.h>

@implementation CompressionTestObject

WCDB_IMPLEMENTATION(CompressionTestObject)
WCDB_SYNTHESIZE(mainId);
WCDB_SYNTHESIZE(subId);
WCDB_SYNTHESIZE(text);
WCDB_SYNTHESIZE(textMatchId);
WCDB_SYNTHESIZE(blob);
WCDB_SYNTHESIZE(blobMatchId);

WCDB_PRIMARY_AUTO_INCREMENT(mainId);

- (BOOL)isEqual:(NSObject*)other
{
    if (other.class != self.class) {
        return NO;
    }
    CompressionTestObject* otherObj = (CompressionTestObject*) other;
    bool ret = _mainId == otherObj.mainId && _subId == otherObj.subId && ([_text isEqualToString:otherObj.text] || _text == otherObj.text) && ([_blob isEqualToData:otherObj.blob] || _blob == otherObj.blob) && _textMatchId == otherObj.textMatchId && _blobMatchId == otherObj.blobMatchId;
    if (!ret) {
        return false;
    }
    return true;
}

@end
