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

#import "ObjectsBasedBenchmark.h"

@interface CipherBenchmark : ObjectsBasedBenchmark
@property (nonatomic, readonly) NSData *password;
@end

@implementation CipherBenchmark {
    NSData *_password;
}

- (NSData *)password
{
    @synchronized(self) {
        if (_password == nil) {
            _password = self.random.data;
        }
        return _password;
    }
}

- (void)doSetUpDatabase
{
    [self.database setCipherKey:self.password];
}

- (void)test_write
{
    [self doTestWrite];
}

- (void)test_read
{
    [self doTestRead];
}

- (void)test_batch_write
{
    [self doTestBatchWrite];
}

#pragma mark - ReusableFactoryPreparation
- (BOOL)willStartPreparing:(NSString *)path
{
    [[[WCTDatabase alloc] initWithPath:path] setCipherKey:self.password];
    return YES;
}

- (NSString *)category
{
    return @"Cipher";
}

- (double)getQuality:(NSString *)path
{
    [[[WCTDatabase alloc] initWithPath:path] setCipherKey:self.password];
    return [super getQuality:path];
}

@end
