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

#import "Factory.h"
#import "NSObject+TestCase.h"
#import "TestCaseLog.h"

@interface Factory ()
@property (nonatomic, readonly) NSFileManager* fileManager;
@property (nonatomic, readonly) NSString* prototype;
@property (nonatomic, readonly) NSArray<NSString*>* prototypes;
@end

@implementation Factory {
    NSString* _prototype;
    NSArray<NSString*>* _prototypes;
}

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super init]) {
        _directory = directory;
        _fileManager = [NSFileManager defaultManager];
    }
    return self;
}

- (void)reset
{
    _prototype = nil;
    _prototypes = nil;
}

- (void)setDelegate:(id<FactoryPreparation>)delegate
{
    _delegate = delegate;
    [self reset];
}

- (void)setQuality:(double)expectedQuality
{
    _quality = expectedQuality;
    [self reset];
}

- (NSString*)prototype
{
    @synchronized(self) {
        if (_prototype == nil) {
            NSString* fileName = [NSString stringWithFormat:@"%@_%@", self.delegate.category, [NSNumber numberWithDouble:self.quality]];
            _prototype = [self.directory stringByAppendingPathComponent:fileName];
        }
        return _prototype;
    }
}

- (NSArray<NSString*>*)prototypes
{
    @synchronized(self) {
        if (_prototypes == nil) {
            NSMutableArray* prototypes = [NSMutableArray arrayWithObject:_prototype];
            if ([self.delegate respondsToSelector:@selector(additionalPrototypes:)]) {
                [prototypes addObjectsFromArray:[self.delegate additionalPrototypes:_prototype]];
            }
            _prototypes = [NSArray arrayWithArray:prototypes];
        }
        return _prototypes;
    }
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
        expired = quality > self.quality * (1.0f + self.tolerance) || quality < self.quality * (1.0f - self.tolerance);
        if (expired) {
            TestCaseLog(@"Prototype is expired: %f/%f", quality, self.quality);
        }
    }
    return expired;
}

- (NSString*)produce:(NSString*)destination
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

    TestCaseLog(@"Prototype at %@", self.prototype);

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
    TestCaseLog(@"Production at %@", product);
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
        if (![self willStartPreparing]) {
            return NO;
        }
        do {
            if (![self.delegate stepPreparePrototype:self.prototype]) {
                return NO;
            }
            quality = [self.delegate getQuality:self.prototype];

            // progress
            double newProgress = quality / self.quality;
            if (newProgress - progress > 0.05f) {
                if (newProgress > 1.0f) {
                    newProgress = 1.0f;
                }
                progress = newProgress;
                TestCaseLog(@"Preparing %.2f%%", progress * 100.0f);
            }
        } while (quality < self.quality * (1.0f - self.tolerance));
        if (![self willEndPreparing]) {
            return NO;
        }
    } while (quality > self.quality * (1.0f + self.tolerance));
    return YES;
}

- (BOOL)willStartPreparing
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(willStartPreparing:)]) {
        if (![self.delegate willStartPreparing:self.prototype]) {
            return NO;
        }
    }
    return YES;
}

- (BOOL)willEndPreparing
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(willEndPreparing:)]) {
        if (![self.delegate willEndPreparing:self.prototype]) {
            return NO;
        }
    }
    return YES;
}

@end
