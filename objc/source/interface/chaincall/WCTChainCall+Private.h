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

#import <WCDB/WCTChainCall.h>

NS_ASSUME_NONNULL_BEGIN

class WCTUsedUpInvalidateGuard final {
public:
    WCTUsedUpInvalidateGuard(WCTChainCall* chaincall);
    ~WCTUsedUpInvalidateGuard();
    WCTUsedUpInvalidateGuard() = delete;
    WCTUsedUpInvalidateGuard(const WCTUsedUpInvalidateGuard&) = delete;
    WCTUsedUpInvalidateGuard& operator=(const WCTUsedUpInvalidateGuard&) = delete;

private:
    WCTChainCall* m_chaincall;
};

@interface WCTChainCall () {
@protected
    WCTHandle* _handle;
@private
    BOOL _invalidateWhenUsedUp;
}

- (instancetype)init NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithHandle:(WCTHandle*)handle NS_DESIGNATED_INITIALIZER;

- (instancetype)invalidateWhenUsedUp;

- (void)tryAlreadyUsedUpInvalidate;

@end

NS_ASSUME_NONNULL_END
