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

#import "BenchmarkFactory.h"
#import "Console.h"
#import "Random.h"
#import "StableRandom.h"
#import "TestCaseLog.h"

typedef NS_ENUM(NSUInteger, BenchmarkFactoryProductionLine) {
    BenchmarkFactoryProductionLineFileSize,
    BenchmarkFactoryProductionLineObjects,
    BenchmarkFactoryProductionLineTables,
};

@interface BenchmarkFactory ()
@property (nonatomic, retain) Random* random;
@property (nonatomic, retain) NSNumber* parameter;
@property (nonatomic, assign) BenchmarkFactoryProductionLine line;
@end

@implementation BenchmarkFactory

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super init]) {
        _directory = directory;
        _renew = NO;
    }
    return self;
}

- (NSString*)getSlotedPath
{
    return [[[self.directory stringByAppendingPathComponent:[self getSlotedKey]] stringByAppendingPathComponent:self.parameter.stringValue] stringByAppendingPathComponent:@"database"];
}

- (BOOL)isProductExpired:(NSString*)path
{
    BOOL expired = self.renew;
    if (!expired) {
        expired = ![[NSFileManager defaultManager] attributesOfItemAtPath:path error:nil].fileIsImmutable;
    }
    if (!expired) {
        double slotedValue = [self getSlotedValue:path].doubleValue;
        double parameter = self.parameter.doubleValue;
        expired = slotedValue > parameter * (1.0f + self.tolerance) || slotedValue < parameter * (1.0f - self.tolerance);
    }
    return expired;
}

- (BOOL)production:(NSString*)destination
{
    NSString* path = [self getSlotedPath];
    NSFileManager* fileManager = [NSFileManager defaultManager];

    if ([self isProductExpired:path]) {
        // reset immutable
        if ([fileManager fileExistsAtPath:path]
            && ![fileManager setAttributes:@{NSFileImmutable : @(NO)} ofItemAtPath:path error:nil]) {
            return NO;
        }

        if (![self prepare:path]
            || ![fileManager setAttributes:@{NSFileImmutable : @(YES)} ofItemAtPath:path error:nil]) {
            return NO;
        }
    }

    // remove deprecated database
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:destination];
    if (![database removeFiles]) {
        return NO;
    }
    [database invalidate];
    database = nil;

    if (![fileManager copyItemAtPath:path toPath:destination error:nil]
        || ![fileManager setAttributes:@{NSFileImmutable : @(NO)} ofItemAtPath:destination error:nil]) {
        return NO;
    }

    // reuse prepared database
    TestCaseLog(@"Production: %@ %@", [self getSlotedKey], [self getSlotedValue:destination]);
    return YES;
}

- (BOOL)prepare:(NSString*)path
{
    double parameter = self.parameter.doubleValue;
    double progress = 0;
    double slotedValue = 0;
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    do {
        if (![database removeFiles]) {
            return NO;
        }
        self.random = [[StableRandom alloc] init];
        if (self.delegate && [self.delegate respondsToSelector:@selector(databaseWillStartPreparing:)]) {
            [self.delegate databaseWillStartPreparing:database];
        }
        do {
            if (![self stepPrepare:database]) {
                return NO;
            }
            slotedValue = [self getSlotedValue:database.path].doubleValue;

            // progress
            double newProgress = slotedValue / parameter;
            if (newProgress - progress > 0.05f) {
                if (newProgress > 1.0f) {
                    newProgress = 1.0f;
                }
                progress = newProgress;
                TestCaseLog(@"Preparing %.2f%%", progress * 100.0f);
            }

        } while (slotedValue < parameter * (1.0f - self.tolerance));
    } while (slotedValue > parameter * (1.0f + self.tolerance));
    return YES;
}

#pragma mark - line
- (NSString*)getSlotedKey
{
    switch (self.line) {
    case BenchmarkFactoryProductionLineFileSize:
        return @"FileSize";
    case BenchmarkFactoryProductionLineObjects:
        return @"Objects";
    case BenchmarkFactoryProductionLineTables:
        return @"Tables";
    }
}

- (NSNumber*)getSlotedValue:(NSString*)path
{
    switch (self.line) {
    case BenchmarkFactoryProductionLineFileSize:
        return [self getSlotedValueForFileSize:path];
    case BenchmarkFactoryProductionLineObjects:
        return [self getSlotedValueForObjects:path];
    case BenchmarkFactoryProductionLineTables:
        return [self getSlotedValueForTables:path];
    }
}

- (BOOL)stepPrepare:(WCTDatabase*)database
{
    switch (self.line) {
    case BenchmarkFactoryProductionLineFileSize:
        return [self stepPrepareForFileSize:database];
    case BenchmarkFactoryProductionLineObjects:
        return [self stepPrepareForObjects:database];
    case BenchmarkFactoryProductionLineTables:
        return [self stepPrepareForTables:database];
    }
}

#pragma mark - File Size
- (void)setProductionLineFileSizeInMB:(NSUInteger)fileSizeInMB
{
    self.line = BenchmarkFactoryProductionLineFileSize;
    self.parameter = @(fileSizeInMB);
}

- (NSNumber*)getSlotedValueForFileSize:(NSString*)path
{
    NSUInteger fileSize = [[[WCTDatabase alloc] initWithPath:path] getFilesSize].value();
    return @((double) fileSize / 1024 / 1024);
}

- (BOOL)stepPrepareForFileSize:(WCTDatabase*)database
{
    int step = self.parameter.intValue * 100;
    NSMutableArray* objects = [NSMutableArray arrayWithCapacity:step];
    for (int i = 0; i < step; ++i) {
        BenchmarkObject* object = [[BenchmarkObject alloc] init];
        object.identifier = i;
        object.content = self.random.data;
        [objects addObject:object];
    }

    return [database runTransaction:^BOOL(WCTHandle* handle) {
               NSString* tableName = [NSString stringWithFormat:@"t_%@", self.random.string];
               return [database createTableAndIndexes:tableName withClass:BenchmarkObject.class]
                      && [handle insertObjects:objects intoTable:tableName];
           }]
           && [database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")];
}

#pragma mark - Objects
- (void)setProductionLineObjects:(NSUInteger)numberOfObjects
{
    self.line = BenchmarkFactoryProductionLineObjects;
    self.parameter = @(numberOfObjects);
}

- (NSNumber*)getSlotedValueForObjects:(NSString*)path
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    if ([database tableExists:@"testTable"]) {
        return [database getValueFromStatement:WCDB::StatementSelect().select(BenchmarkObject.allProperties.count()).from(@"testTable")].numberValue;
    }
    return nil;
}

- (BOOL)stepPrepareForObjects:(WCTDatabase*)database
{
    int numberOfObjects = [self getSlotedValue:database.path].intValue;
    int maxNumberOfObjects = self.parameter.intValue;
    int step = maxNumberOfObjects / 100;
    if (step > maxNumberOfObjects - numberOfObjects) {
        step = maxNumberOfObjects - numberOfObjects;
    }

    NSString* tableName = @"testTable";

    int startIdentifier = [database getValueFromStatement:WCDB::StatementSelect().select(BenchmarkObject.identifier.max()).from(tableName)].numberValue.intValue + 1;
    NSMutableArray* objects = [NSMutableArray arrayWithCapacity:step];
    for (int i = 0; i < step; ++i) {
        BenchmarkObject* object = [[BenchmarkObject alloc] init];
        object.identifier = startIdentifier + i;
        object.content = self.random.data;
        [objects addObject:object];
    }

    return [database runTransaction:^BOOL(WCTHandle* handle) {
               return [database createTableAndIndexes:tableName withClass:BenchmarkObject.class]
                      && [handle insertObjects:objects intoTable:tableName];
           }]
           && [database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")];
}

#pragma mark - Tables
- (void)setProductionLineTables:(NSUInteger)numberOfTables
{
    self.line = BenchmarkFactoryProductionLineTables;
    self.parameter = @(numberOfTables);
}

- (NSNumber*)getSlotedValueForTables:(NSString*)path
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    return [database getValueFromStatement:WCDB::StatementSelect().select(WCTMaster.allProperties.count()).from(WCTMaster.tableName).where(WCTMaster.type == "table")].numberValue;
}

- (BOOL)stepPrepareForTables:(WCTDatabase*)database
{
    int numberOfTables = [self getSlotedValue:database.path].intValue;
    int maxNumberOfTables = self.parameter.intValue;
    int step = maxNumberOfTables / 100;
    if (step > maxNumberOfTables - numberOfTables) {
        step = maxNumberOfTables - numberOfTables;
    }

    NSMutableArray* objects = [NSMutableArray arrayWithCapacity:step];
    for (int i = 0; i < step; ++i) {
        BenchmarkObject* object = [[BenchmarkObject alloc] init];
        object.identifier = 1;
        object.content = self.random.data;
        [objects addObject:object];
    }

    return [database runTransaction:^BOOL(WCTHandle* handle) {
               for (BenchmarkObject* object in objects) {
                   NSString* tableName = [NSString stringWithFormat:@"t_%@", self.random.string];
                   if (![database createTableAndIndexes:tableName withClass:BenchmarkObject.class]
                       || ![handle insertObject:object intoTable:tableName]) {
                       return NO;
                   }
               }
               return YES;
           }]
           && [database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")];
}

@end
