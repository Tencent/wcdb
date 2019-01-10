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
@property (nonatomic, readonly) NSFileManager* fileManager;
@property (nonatomic, readonly) NSString* prototype;
@property (nonatomic, readonly) NSArray<NSString*>* prototypes;
@property (nonatomic, readonly) NSDictionary<NSString*, NSString*>* parameters;
@end

@implementation ReusableFactory

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super init]) {
        _directory = directory;
        _fileManager = [NSFileManager defaultManager];
    }
    return self;
}

- (void)setDelegate:(id<ReusableFactoryPreparation>)delegate
{
    _delegate = delegate;

    _parameters = [NSDictionary dictionaryWithObjectsAndKeys:
                                self.delegate.category, @"category", @(self.expectedQuality), @"quality", nil];

    _prototype = [self.directory stringByAppendingPathComponent:[NSString stringWithFormat:@"%lu", _parameters.description.hash]];

    NSMutableArray* prototypes = [NSMutableArray arrayWithObject:_prototype];
    if ([self.delegate respondsToSelector:@selector(additionalPrototypes:)]) {
        [prototypes addObjectsFromArray:[self.delegate additionalPrototypes:_prototype]];
    }
    _prototypes = [NSArray arrayWithArray:prototypes];
}

- (BOOL)removePrototypes
{
    return [self.fileManager setFileImmutable:NO ofItemsIfExistsAtPaths:self.prototypes error:nil] && [self.fileManager removeItemsIfExistsAtPaths:self.prototypes error:nil];
}

- (BOOL)isPrototypeExpired
{
    BOOL expired = ![self.fileManager isFileImmutableOfItemAtPath:self.prototype error:nil];
    if (!expired) {
        double quality = [self.delegate getQuality:self.prototype];
        expired = quality > self.expectedQuality * (1.0f + self.tolerance) || quality < self.expectedQuality * (1.0f - self.tolerance);
    }
    return expired;
}

- (NSString*)production:(NSString*)destination
{
    if ([self isPrototypeExpired]) {
        // remove old prototypes
        if (![self.fileManager setFileImmutable:NO ofItemsIfExistsAtPaths:self.prototypes error:nil]) {
            return nil;
        }

        if (![self prepare]) {
            return nil;
        }

        // prepare and set immutable
        if (![self.fileManager setFileImmutable:YES ofItemsIfExistsAtPaths:self.prototypes error:nil]) {
            return nil;
        }
    }

    NSArray<NSString*>* products = [NSString pathsByReplacingPaths:self.prototypes withDirectory:destination];
    // reset immutable for dirty files
    if (![self.fileManager setFileImmutable:NO ofItemsIfExistsAtPaths:products error:nil]) {
        return nil;
    }
    if (![self.fileManager copyItemsIfExistsAtPaths:self.prototypes toDirectory:destination error:nil]
        || ![self.fileManager setFileImmutable:NO ofItemsIfExistsAtPaths:products error:nil]) {
        return nil;
    }

    NSString* product = [NSString pathByReplacingPath:self.prototype withDirectory:destination];
    TestCaseLog(@"Production: %@ %@", self.delegate.category, @([self.delegate getQuality:product]));
    return product;
}

- (BOOL)prepare
{
    double progress = 0;
    double quality = 0;
    do {
        if (![self.fileManager removeItemsIfExistsAtPaths:self.prototypes error:nil]) {
            return NO;
        }
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
        if (self.delegate && [self.delegate respondsToSelector:@selector(willEndPreparing:)]) {
            [self.delegate willEndPreparing:self.prototype];
        }
    } while (quality > self.expectedQuality * (1.0f + self.tolerance));
    return YES;
}

@end
