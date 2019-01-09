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

#import <TestCase/NSObject+TestCase.h>
#import <TestCase/ReusableFactory.h>
#import <TestCase/TestCaseLog.h>

@interface ReusableFactory ()
@property (nonatomic, readonly) NSString* prototype;
@property (nonatomic, readonly) NSArray<NSString*>* prototypes;
@property (nonatomic, readonly) NSDictionary<NSString*, NSString*>* parameters;

@end

@implementation ReusableFactory

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super init]) {
        _directory = directory;
        _renew = NO;
    }
    return self;
}

- (void)setDelegate:(id<ReusableFactoryPreparation>)delegate
{
    _delegate = delegate;

    NSMutableDictionary* parameters = [NSMutableDictionary dictionary];
    [parameters setObject:self.delegate.category forKey:@"category"];
    [parameters setObject:@(self.expectedQuality) forKey:@"quality"];
    ;
    if ([self.delegate respondsToSelector:@selector(additionalParameters)]) {
        NSDictionary<NSString*, NSString*>* additionalParameters = self.delegate.additionalParameters;
        for (NSString* key in parameters.allKeys) {
            [parameters setObject:additionalParameters[key] forKey:key];
        }
    }
    _parameters = [NSDictionary dictionaryWithDictionary:parameters];

    _prototype = [self.directory stringByAppendingPathComponent:[NSString stringWithFormat:@"%lu", _parameters.description.hash]];

    NSMutableArray* prototypes = [NSMutableArray arrayWithObject:_prototype];
    if ([self.delegate respondsToSelector:@selector(additionalPrototypes:)]) {
        [prototypes addObjectsFromArray:[self.delegate additionalPrototypes:_prototype]];
    }
    _prototypes = [NSArray arrayWithArray:_prototypes];
}

- (BOOL)isPrototypeExpired
{
    NSString* prototype = self.prototype;
    BOOL expired = self.renew;
    if (!expired) {
        expired = ![[NSFileManager defaultManager] isFileImmutableOfItemAtPath:prototype error:nil];
    }
    if (!expired) {
        double quality = [self.delegate getQuality:prototype];
        expired = quality > self.expectedQuality * (1.0f + self.tolerance) || quality < self.expectedQuality * (1.0f - self.tolerance);
    }
    return expired;
}

- (NSString*)production:(NSString*)destination
{
    NSFileManager* fileManager = [NSFileManager defaultManager];

    if ([self isPrototypeExpired]) {
        // reset immutable
        if (![fileManager setFileImmutable:NO ofItemsIfExistsAtPaths:self.prototypes error:nil]) {
            return nil;
        }

        if (![self prepare]
            || ![fileManager setFileImmutable:YES ofItemsIfExistsAtPaths:self.prototypes error:nil]) {
            return nil;
        }
    }

    if (![fileManager copyItemsIfExistsAtPaths:self.prototypes toDirectory:destination error:nil]) {
        return nil;
    }

    NSString* product = [NSString pathByReplacingPath:self.prototype withDirectory:destination];
    TestCaseLog(@"Production: %@ %@", self.delegate.category, [self.delegate getQuality:product]);
    return product;
}

- (BOOL)prepare
{
    if (![[NSFileManager defaultManager] removeItemsIfExistsAtPaths:self.prototypes error:nil]) {
        return NO;
    }

    double progress = 0;
    double quality = 0;
    do {
        if (self.delegate && [self.delegate respondsToSelector:@selector(willStartPreparing:)]) {
            [self.delegate willStartPreparing:self.prototype];
        }
        do {
            if (![self.delegate stepPreparePrototype:self.prototype]) {
                return NO;
            }
            quality = [self.delegate getQuality:self.prototype];

            // progress
            double newProgress = quality / self.expectedQuality;
            if (newProgress - progress > 0.05f) {
                if (newProgress > 1.0f) {
                    newProgress = 1.0f;
                }
                progress = newProgress;
                TestCaseLog(@"Preparing %.2f%%", progress * 100.0f);
            }

        } while (quality < self.expectedQuality * (1.0f - self.tolerance));
    } while (quality > self.expectedQuality * (1.0f + self.tolerance));
    return YES;
}

@end
