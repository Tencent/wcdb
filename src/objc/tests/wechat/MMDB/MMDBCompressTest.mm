//
// Created by qiuwenchen on 2024/1/17.
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

#import "MMDBMainData.h"
#import "TestCase.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCTDatabase+Test.h>

typedef enum : NSUInteger {
    MsgDictType_brandMsg = 1,
    MsgDictType_brandExt,
    MsgDictType_msgExt,
    MsgDictType_textMsg,
    MsgDictType_imageMsg,
    MsgDictType_voiceMsg,
    MsgDictType_videoMsg,
    MsgDictType_appMsg,
    MsgDictType_emojiMsg,
    MsgDictType_NewXmlMsg,
    MsgDictType_OtherMsg,
    MsgDictType_Count,
} MsgDictType;

@interface MMDBCompressTest : Benchmark

@property (nonatomic, assign) BOOL dictLoaded;
@property (nonatomic, strong) NSString* workSpace;
@property (nonatomic, strong) NSArray<NSString*>* msgPaths;
@property (nonatomic, strong) NSArray<NSString*>* brandPaths;
@property (nonatomic, strong) NSDictionary<NSString*, NSNumber*>* maxIds;
@property (nonatomic, strong) NSArray<NSString*>* largeTables;
@property (nonatomic, assign) int testQuality;

@end

@implementation MMDBCompressTest

- (void)setUp
{
    [super setUp];
    self.workSpace = @"/Users/qiuwenchen/Desktop/ZSTDTest";
//    self.workSpace = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject stringByAppendingPathComponent:@"ZSTDTest"];
    self.testQuality = 1000;
    [self tryRegisterDict];

    if (self.msgPaths.count == 0) {
        NSMutableArray* pathes = [[NSMutableArray alloc] init];
        for (int i = 1; i < 6; i++) {
            for (int j = 1; j < 5; j++) {
                if ((i == 1 && j == 2) || (i == 2 && j == 4) || (i == 5 && j == 2)) {
                    continue;
                }
                [pathes addObject:[self.workSpace stringByAppendingFormat:@"/mmdb/mmdb%d/message_%d.sqlite", i, j]];
            }
        }
        self.msgPaths = pathes;
    }

    if (self.brandPaths.count == 0) {
        NSMutableArray* pathes = [[NSMutableArray alloc] init];
        for (int i = 1; i < 8; i++) {
            [pathes addObject:[self.workSpace stringByAppendingFormat:@"/Brand/Brand%d/BrandMsg.db", i]];
        }
        self.brandPaths = pathes;
    }
}

- (void)tryRegisterDict
{
    if (_dictLoaded) {
        return;
    }
    NSString* dictFolder = [self.workSpace stringByAppendingPathComponent:@"msgDict"];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"BrandDict.zstd"] withDictId:MsgDictType_brandMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"BrandExtDict.zstd"] withDictId:MsgDictType_brandExt];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"TextMsgDict.zstd"] withDictId:MsgDictType_textMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"ExtMsgDict.zstd"] withDictId:MsgDictType_msgExt];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"ImageMsgDict.zstd"] withDictId:MsgDictType_imageMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"AppMsgDict.zstd"] withDictId:MsgDictType_appMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"OtherMsgDict.zstd"] withDictId:MsgDictType_OtherMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"VideoMsgDict.zstd"] withDictId:MsgDictType_videoMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"VoiceMsgDict.zstd"] withDictId:MsgDictType_voiceMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"EmojiMsgDict.zstd"] withDictId:MsgDictType_emojiMsg];
    [self registerDictAtPath:[dictFolder stringByAppendingPathComponent:@"NewXmlMsgDict.zstd"] withDictId:MsgDictType_NewXmlMsg];
    _dictLoaded = YES;
}

- (void)registerDictAtPath:(NSString*)path withDictId:(WCTDictId)dictId
{
    NSData* dict = [NSData dataWithContentsOfFile:path];
    TestCaseAssertTrue([WCTDatabase registerZSTDDict:dict andDictId:dictId]);
}

- (void)configCompression:(WCTDatabase*)database
{
    if (![database.path containsString:@"BrandMsg"]) {
        [database setCompressionWithFilter:^(WCTCompressionUserInfo* info) {
            if ([info.table hasPrefix:@"Chat_"]) {
                [info addZSTDDictCompressProperty:DBMessage.message
                                withMatchProperty:DBMessage.type
                                    andMatchDicts:@{
                                        @(MM_DATA_TEXT) : @(MsgDictType_textMsg),
                                        @(MM_DATA_IMG) : @(MsgDictType_imageMsg),
                                        @(MM_DATA_VOICEMSG) : @(MsgDictType_voiceMsg),
                                        @(MM_DATA_VIDEO) : @(MsgDictType_videoMsg),
                                        @(MM_DATA_EMOJI) : @(MsgDictType_emojiMsg),
                                        @(MM_DATA_APPMSG) : @(MsgDictType_appMsg),
                                        @(MM_DATA_SYSCMD_NEWXML) : @(MsgDictType_NewXmlMsg),
                                        @(WCTDictDefaultMatchValue) : @(MsgDictType_OtherMsg),
                                    }];
            } else if ([info.table hasPrefix:@"ChatExt2_"]) {
                [info addZSTDDictCompressProperty:DBMessageExt.msgSource withDictId:MsgDictType_msgExt];
            }
        }];
    } else {
        [database setCompressionWithFilter:^(WCTCompressionUserInfo* info) {
            if ([info.table hasPrefix:@"Chat_"]) {
                [info addZSTDDictCompressProperty:DBMessage.message withDictId:MsgDictType_brandMsg];
            } else if ([info.table hasPrefix:@"ChatExt2_"]) {
                [info addZSTDDictCompressProperty:DBMessageExt.msgSource withDictId:MsgDictType_brandExt];
            }
        }];
    }
}

- (void)testCompressMsg
{
    for (NSString* path in self.msgPaths) {
        @autoreleasepool {
            WCTDatabase* originDB = [[WCTDatabase alloc] initWithPath:path];
            TestCaseAssertTrue([originDB truncateCheckpoint]);
            NSString* tempPath = [self addSubfix:@"temp" toPath:path];
            WCTDatabase* database = [[WCTDatabase alloc] initWithPath:tempPath];
            [database removeFiles];
            [self.fileManager copyItemsIfExistsAtPath:path toPath:tempPath];

            WCTValue* originTotalPageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::pageCount())];
            WCTValue* originTotalFreePageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount())];

            [self configCompression:database];

            while (![database isCompressed]) {
                TestCaseAssertTrue([database stepCompression]);
            }

            WCTValue* currentTotalPageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::pageCount())];
            WCTValue* currentTotalFreePageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount())];

            [self log:@"CompressResult: originPageCount %@, originFreePageCount %@, currentPageCount %@, currentFreePageCount %@, compressRadio %f, path %@",
                      originTotalPageCount,
                      originTotalFreePageCount,
                      currentTotalPageCount,
                      currentTotalFreePageCount,
                      (double) (currentTotalPageCount.numberValue.integerValue - currentTotalFreePageCount.numberValue.intValue) / (originTotalPageCount.numberValue.integerValue - originTotalFreePageCount.numberValue.integerValue),
                      [path stringByReplacingOccurrencesOfString:self.workSpace
                                                      withString:@"~"]];
        }
    }
}

- (void)loadMaxIdAndLargeTables
{
    if (self.maxIds.count > 0) {
        return;
    }
    NSArray* tables = [self.database getColumnOnResultColumn:WCTMaster.name
                                                   fromTable:WCTMaster.tableName
                                                       where:WCTMaster.type == "table"
                                                             && (WCTMaster.name.like("Chat\\_%%").escape("\\"))];
    NSMutableDictionary* maxIds = [[NSMutableDictionary alloc] init];
    NSMutableArray* largeTables = [[NSMutableArray alloc] init];
    for (NSString* table in tables) {
        WCTValue* maxRowid = [self.database getValueOnResultColumn:WCDB::Column::rowid().max() fromTable:table];
        maxIds[table] = maxRowid;
        if (maxRowid.numberValue.integerValue > self.testQuality * 10) {
            [largeTables addObject:table];
        }
    }
    self.maxIds = maxIds;
    self.largeTables = largeTables;
}

- (NSString*)addSubfix:(NSString*)subfix toPath:(NSString*)path
{
    NSString* newPath = [path stringByReplacingOccurrencesOfString:@".db" withString:[NSString stringWithFormat:@"-%@.db", subfix]];
    newPath = [newPath stringByReplacingOccurrencesOfString:@".sqlite" withString:[NSString stringWithFormat:@"-%@.sqlite", subfix]];
    return newPath;
}

- (NSString*)removeSubfix:(NSString*)subfix fromPath:(NSString*)path
{
    return [path stringByReplacingOccurrencesOfString:[@"-" stringByAppendingString:subfix] withString:@""];
}

- (void)tryGenCompressDatabase:(NSString*)path
{
    NSString* compressedPath = [self addSubfix:@"compressed" toPath:path];
    if (![self.fileManager fileExistsAtPath:compressedPath]) {
        [self.fileManager copyItemsIfExistsAtPath:path toPath:compressedPath];
    }
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:compressedPath];
    [self configCompression:database];
    while (![database isCompressed]) {
        TestCaseAssertTrue([database stepCompression]);
    }
    TestCaseAssertTrue([database truncateCheckpoint]);
    [database close];
    self.path = compressedPath;
}

- (void)setupBenchmarkDatabase
{
    NSString* tempPath = [self addSubfix:@"temp" toPath:self.path];
    [self.fileManager copyItemsIfExistsAtPath:self.path toPath:tempPath];
    self.path = tempPath;
}

- (void)tearDownBenchMarkDatabase
{
    if ([self.database.path containsString:@"temp"]) {
        [self.database removeFiles];
    }
    self.path = [self removeSubfix:@"temp" fromPath:self.path];
}

- (void)doTestWrite
{
    [self loadMaxIdAndLargeTables];
    NSArray<DBMessage*>* objects = [self.database getObjectsOfClass:DBMessage.class fromTable:self.largeTables.firstObject limit:self.testQuality];
    for (DBMessage* object in objects) {
        object.isAutoIncrement = YES;
        object.msgLocalId = 0;
    }
    TestCaseAssertTrue(objects.count > self.testQuality / 2);
    __block BOOL result;
    [self
    doMeasure:^{
        NSString* tableName = self.maxIds.allKeys[Random.shared.int16 % self.maxIds.count];
        for (DBMessage* object in objects) {
            if (![self.database insertObject:object intoTable:tableName]) {
                result = NO;
                return;
            }
        }
        result = YES;
    }
    setUp:^{
        [self setupBenchmarkDatabase];
    }
    tearDown:^{
        [self tearDownBenchMarkDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)doTestRead
{
    [self loadMaxIdAndLargeTables];
    __block NSMutableArray<DBMessage*>* result;
    [self
    doMeasure:^{
        NSString* table = self.largeTables[Random.shared.int16 % self.largeTables.count];
        for (int i = 1; i <= self.testQuality; i++) {
            DBMessage* obj = [self.database getObjectOfClass:DBMessage.class fromTable:table where:DBMessage.msgLocalId == i];
            [result addObject:obj];
        }
    }
    setUp:^{
        [self setupBenchmarkDatabase];
    }
    tearDown:^{
        [self tearDownBenchMarkDatabase];
        result = [NSMutableArray new];
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result.count > self.testQuality / 2);
    }];
}

- (void)doTestBatchRead
{
    [self loadMaxIdAndLargeTables];
    __block NSArray<DBMessage*>* result;
    [self
    doMeasure:^{
        NSString* table = self.largeTables[Random.shared.int16 % self.largeTables.count];
        result = [self.database getObjectsOfClass:DBMessage.class fromTable:table limit:self.testQuality];
    }
    setUp:^{
        [self setupBenchmarkDatabase];
    }
    tearDown:^{
        [self tearDownBenchMarkDatabase];
        result = nil;
    }
    checkCorrectness:^{
        TestCaseAssertEqual(result.count, self.testQuality);
    }];
}

- (void)doTestBatchWrite
{
    [self loadMaxIdAndLargeTables];
    NSArray<DBMessage*>* objects = [self.database getObjectsOfClass:DBMessage.class fromTable:self.largeTables.firstObject limit:self.testQuality];
    for (DBMessage* object in objects) {
        object.isAutoIncrement = YES;
        object.msgLocalId = 0;
    }
    __block BOOL result;
    [self
    doMeasure:^{
        NSString* table = self.maxIds.allKeys[Random.shared.int16 % self.maxIds.count];
        result = [self.database insertObjects:objects intoTable:table];
    }
    setUp:^{
        [self setupBenchmarkDatabase];
    }
    tearDown:^{
        [self tearDownBenchMarkDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)doTestRandomRead
{
    [self loadMaxIdAndLargeTables];
    __block NSMutableArray<DBMessage*>* result = [[NSMutableArray alloc] initWithCapacity:self.testQuality];
    __block NSMutableArray<WCTValue*>* identifiers;
    __block NSString* table;
    [self
    doMeasure:^{
        int index = 0;
        for (int i = 1; i <= self.testQuality; i++) {
            index = [[Random shared] uint32] % identifiers.count;
            DBMessage* obj = [self.database getObjectOfClass:DBMessage.class fromTable:table where:DBMessage.msgLocalId == identifiers[index].numberValue.intValue];
            if (obj == nil) {
                continue;
            }
            [result addObject:obj];
            [identifiers removeObjectAtIndex:index];
        }
    }
    setUp:^{
        [self setupBenchmarkDatabase];
    }
    tearDown:^{
        table = self.largeTables[Random.shared.int16 % self.largeTables.count];
        identifiers = (NSMutableArray<WCTValue*>*) [self.database getColumnOnResultColumn:DBMessage.msgLocalId fromTable:table];
        [self tearDownBenchMarkDatabase];
        result = [[NSMutableArray alloc] initWithCapacity:self.testQuality];
    }
    checkCorrectness:^{
        TestCaseAssertEqual(result.count, self.testQuality);
    }];
}

- (void)doTestRandomWrite
{
    [self loadMaxIdAndLargeTables];
    NSArray<DBMessage*>* objects = [self.database getObjectsOfClass:DBMessage.class fromTable:self.largeTables.firstObject limit:self.testQuality];
    for (DBMessage* object in objects) {
        object.isAutoIncrement = YES;
        object.msgLocalId = 0;
    }
    [self
    doMeasure:^{
        for (DBMessage* msg in objects) {
            NSString* table = self.maxIds.allKeys[Random.shared.int16 % self.maxIds.count];
            TestCaseAssertTrue([self.database insertObject:msg intoTable:table]);
        }
    }
    setUp:^{
        [self setupBenchmarkDatabase];
    }
    tearDown:^{
        [self tearDownBenchMarkDatabase];
    }
    checkCorrectness:^{
    }];
}

- (void)testBrandWrite
{
    self.path = self.brandPaths.firstObject;
    [self doTestWrite];
}

- (void)testBrandRead
{
    self.path = self.brandPaths.firstObject;
    [self doTestRead];
}

- (void)testBrandBatchRead
{
    self.path = self.brandPaths.firstObject;
    [self doTestBatchRead];
}

- (void)testBrandBatchWrite
{
    self.path = self.brandPaths.firstObject;
    [self doTestBatchWrite];
}

- (void)testBrandRandomRead
{
    self.path = self.brandPaths.firstObject;
    [self doTestRandomRead];
}

- (void)testBrandRandomWrite
{
    self.path = self.brandPaths.firstObject;
    [self doTestRandomWrite];
}

- (void)testBrandCompressWrite
{
    self.path = self.brandPaths.firstObject;
    [self tryGenCompressDatabase:self.path];
    [self doTestWrite];
}

- (void)testBrandCompressRead
{
    self.path = self.brandPaths.firstObject;
    [self tryGenCompressDatabase:self.path];
    [self doTestRead];
}

- (void)testBrandCompressBatchRead
{
    self.path = self.brandPaths.firstObject;
    [self tryGenCompressDatabase:self.path];
    [self doTestBatchRead];
}

- (void)testBrandCompressBatchWrite
{
    self.path = self.brandPaths.firstObject;
    [self tryGenCompressDatabase:self.path];
    [self doTestBatchWrite];
}

- (void)testBrandCompressRandomRead
{
    self.path = self.brandPaths.firstObject;
    [self tryGenCompressDatabase:self.path];
    [self doTestRandomRead];
}

- (void)testBrandCompressRandomWrite
{
    self.path = self.brandPaths.firstObject;
    [self tryGenCompressDatabase:self.path];
    [self doTestRandomWrite];
}

@end
