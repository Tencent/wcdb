//
// Created by 陈秋文 on 2023/11/5.
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
#import <zstd/zdict.h>
#import <zstd/zstd.h>

@interface CompressBenchmark : NSObject

@property (nonatomic, strong) NSString* compressDB;
@property (nonatomic, strong) NSString* compressId;
@property (nonatomic, assign) double compressRadio;
@property (nonatomic, assign) int64_t compressTimeInUs;
@property (nonatomic, assign) int64_t decompressTimeInUs;

@end

@interface ZDict : NSObject
@property (nonatomic, strong) NSData* dict;
@property (nonatomic, assign) ZSTD_CDict* cDict;
@property (nonatomic, assign) ZSTD_DDict* dDict;
@property (nonatomic, assign) int trainCount;
@property (nonatomic, assign) int compressCount;
@property (nonatomic, assign) int64_t preLength;
@property (nonatomic, assign) int64_t compressLength;
@end

@implementation ZDict
- (void)dealloc
{
    if (_cDict != nullptr) {
        ZSTD_freeCDict(_cDict);
    }
    if (_dDict != nullptr) {
        ZSTD_freeDDict(_dDict);
    }
}
@end

@implementation CompressBenchmark
@end

typedef enum : NSUInteger {
    DictType_msgCommon = 1,
    DictType_msgExt,
    DictType_Text,
    DictType_ImageMsg,
    DictType_Voice,
    DictType_Video,
    DictType_App,
    DictType_Emoji,
    DictType_NEWXML,
    DictType_Link,
    DictType_Record,
    DictType_Ref,
    DictType_File,
    DictType_WeApp,
    DictType_Finder,
    DictType_OpenSDKWeApp,
    DictType_Note,
    DictType_OtherMsg,
    DictType_Count,
} DictType;

@interface ZSTDBenchmark : Benchmark

@property (nonatomic, strong) NSMutableDictionary<NSNumber*, ZDict*>* dicts;

@property (nonatomic, assign) ZSTD_CCtx* cctx;
@property (nonatomic, assign) ZSTD_DCtx* dctx;

@property (nonatomic, assign) bool usingRandomDict;
@property (nonatomic, assign) bool usingCommonDict;
@property (nonatomic, assign) bool usingTypedDict;
@property (nonatomic, assign) bool usingTypedDictForAppMsg;
@property (nonatomic, assign) int compressLevel;
@property (nonatomic, assign) int dictSize;
@property (nonatomic, assign) int dictMsgCount;

@property (nonatomic, assign) int64_t compressTime;
@property (nonatomic, assign) int64_t compressCount;
@property (nonatomic, assign) int64_t decompressTime;
@property (nonatomic, assign) int64_t decompressCount;

@property (nonatomic, strong) WCTDatabase* compressedDatabase;

@property (nonatomic, strong) NSArray* allTables;

@property (nonatomic, strong) CompressBenchmark* currentBenchmark;
@property (nonatomic, strong) NSMutableDictionary<NSString*, NSMutableArray<CompressBenchmark*>*>* benchmarks;

@property (nonatomic, strong) NSArray* allDatabasePaths;

@property (nonatomic, strong) NSString* desktop;

@end

@implementation ZSTDBenchmark

- (void)setUp
{
    [super setUp];
    self.desktop = @"/Users/chenqiuwen/Desktop";
    NSMutableArray* paths = [[NSMutableArray alloc] init];
    NSString* dir = [self.desktop stringByAppendingString:@"/ZSTDTest/"];
    for (int i = 1; i < 7; i++) {
        [paths addObject:[dir stringByAppendingFormat:@"Brand/Brand%d/BrandMsg.db", i]];
    }
    for (int i = 1; i < 5; i++) {
        for (int j = 1; j < 5; j++) {
            if ((i == 1 && j == 2) || (i == 2 && j == 4)) {
                continue;
            }
            [paths addObject:[dir stringByAppendingFormat:@"mmdb/mmdb%d/message_%d.sqlite", i, j]];
        }
    }
    self.allDatabasePaths = paths;
    self.benchmarks = [[NSMutableDictionary alloc] init];
    self.dicts = [[NSMutableDictionary alloc] init];
    _cctx = nullptr;
    _dctx = nullptr;
}

- (NSString*)getCompressDatabasePath:(NSString*)databasePath
{
    NSString* fileName = [[databasePath lastPathComponent] componentsSeparatedByString:@"."][0];
    NSString* compressedFileName;
    compressedFileName = [fileName stringByAppendingString:@"Compress"];
    return [databasePath stringByReplacingOccurrencesOfString:fileName withString:compressedFileName];
}

- (bool)isExtTable:(NSString*)tableName
{
    return [tableName hasPrefix:@"ChatExt2"];
}

- (void)setupCtx
{
    if (_cctx != nullptr) {
        ZSTD_freeCCtx(_cctx);
    }
    _cctx = ZSTD_createCCtx();
    ZSTD_CCtx_setParameter(_cctx, ZSTD_c_compressionLevel, self.compressLevel);
    if (_dctx != nullptr) {
        ZSTD_freeDCtx(_dctx);
    }
    _dctx = ZSTD_createDCtx();
}

- (void)testTrainBigDict
{
    NSMutableString* content = [[NSMutableString alloc] init];
    std::vector<size_t> sizes;
    int64_t remainSize = (unsigned) -1;
    [self readDataFromFile:[self.desktop stringByAppendingPathComponent:@"BrandXml/20231125群发xml.csv"] toContent:content withSizeRecord:sizes remainSize:remainSize];
    [self readDataFromFile:[self.desktop stringByAppendingPathComponent:@"BrandXml/20231126群发xml.csv"] toContent:content withSizeRecord:sizes remainSize:remainSize];
    size_t bufferSize = 100 * 1024;
    void* dictBuffer = malloc(bufferSize);
    size_t dictSize = ZDICT_trainFromBuffer(dictBuffer, bufferSize, content.UTF8String, sizes.data(), (unsigned int) sizes.size());
    XCTAssertFalse(ZDICT_isError(dictSize));
    dictSize = ZDICT_finalizeDictionary(dictBuffer, bufferSize, dictBuffer, dictSize, content.UTF8String, sizes.data(), (unsigned int) sizes.size(), { self.compressLevel, 0, 1 });
    NSData* data = [[NSData alloc] initWithBytes:dictBuffer length:dictSize];
    XCTAssertTrue([data writeToFile:[self.desktop stringByAppendingPathComponent:@"BrandDict.zstd"] atomically:YES]);
}

- (void)readDataFromFile:(NSString*)path toContent:(NSMutableString*)content withSizeRecord:(std::vector<size_t>&)sizes remainSize:(int64_t&)remainSize
{
    @autoreleasepool {
        NSString* allContent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:NULL];
        ZSTD_DCtx* ctx = ZSTD_createDCtx();
        size_t bufferSize = 1024 * 1024;
        void* buffer = malloc(bufferSize);
        TestCaseAssertNotNil(buffer);
        for (NSString* line in [allContent componentsSeparatedByString:@"\n"]) {
            @autoreleasepool {
                NSData* data = [[NSData alloc] initWithBase64EncodedString:line options:0];
                if (data == nil) {
                    continue;
                }
                size_t decompressSize = ZSTD_decompressDCtx(ctx, buffer, bufferSize, data.bytes, data.length);
                if (ZSTD_isError(decompressSize)) {
                    continue;
                }
                remainSize -= decompressSize;
                if (remainSize < 0) {
                    break;
                }
                NSString* xml = [[NSString alloc] initWithBytes:buffer length:decompressSize encoding:NSUTF8StringEncoding];
                if (xml.length < 10) {
                    continue;
                }
                [content appendString:xml];
                sizes.push_back(decompressSize);
            }
        }
    }
}

- (void)testCompressWithBigDict
{
    self.compressLevel = ZSTD_defaultCLevel();
    self.skipDebugLog = YES;
    NSData* dictData = [NSData dataWithContentsOfFile:[self.desktop stringByAppendingPathComponent:@"BrandDict.zstd"]];
    ZDict* dict = [[ZDict alloc] init];
    dict.cDict = ZSTD_createCDict(dictData.bytes, dictData.length, self.compressLevel);
    dict.dDict = ZSTD_createDDict(dictData.bytes, dictData.length);
    self.dicts[@(DictType_msgCommon)] = dict;

    self.usingCommonDict = true;
    self.usingTypedDict = false;
    self.dictSize = 100 * 1024;
    self.dictMsgCount = 200000;
    for (NSString* path in self.allDatabasePaths) {
        if (![path containsString:@"BrandMsg"]) {
            continue;
        }
        if (self.dicts[@(DictType_msgExt)] == nil) {
            [self setPath:path];
            self.allTables = [self.database getColumnOnResultColumn:WCTMaster.name
                                                          fromTable:WCTMaster.tableName
                                                              where:WCTMaster.type == "table"
                                                                    && (WCTMaster.name.like("Chat\\_%%").escape("\\") || WCTMaster.name.like("ChatExt2\\_%%").escape("\\"))];

            NSMutableDictionary<NSNumber*, NSMutableArray<NSString*>*>* trainingContents = [[NSMutableDictionary alloc] init];
            for (int i = 0; i < DictType_Count; i++) {
                trainingContents[@(i)] = [[NSMutableArray alloc] init];
            }
            [self findTrainingMessage:trainingContents inTables:self.allTables from:self.database];
            ZDict* extDict = [self tranDictWith:trainingContents[@(DictType_msgExt)] dictId:(unsigned) DictType_msgExt];
            self.dicts[@(DictType_msgExt)] = extDict;
        }
        @autoreleasepool {
            self.allTables = nil;
            [self commonTestWithPath:path andId:[NSString stringWithFormat:@"%d-%d-%d", self.usingTypedDict, self.usingCommonDict, self.dictMsgCount]];
        }
    }
    [self printBenchmark];
}

- (void)testCompressBrand
{
    self.compressLevel = ZSTD_defaultCLevel();
    self.skipDebugLog = YES;

    self.usingCommonDict = true;
    self.usingTypedDict = false;
    self.dictSize = 100 * 1024;
    self.dictMsgCount = 10000000;
    for (NSString* path in self.allDatabasePaths) {
        if (![path containsString:@"BrandMsg"]) {
            continue;
        }
        @autoreleasepool {
            self.allTables = nil;
            [self commonTestWithPath:path andId:[NSString stringWithFormat:@"%d-%d-%d", self.usingTypedDict, self.usingCommonDict, self.dictMsgCount]];
        }
    }
    [self printBenchmark];
    NSData* brandDict = self.dicts[@(DictType_msgCommon)].dict;
    XCTAssertTrue([brandDict writeToFile:@"~/Desktop/BrandDict.zstd" atomically:YES]);
    NSData* extDict = self.dicts[@(DictType_msgExt)].dict;
    XCTAssertTrue([extDict writeToFile:@"~/Desktop/BrandExtDict.zstd" atomically:YES]);
}

- (ZDict*)tranDictWith:(NSArray<NSString*>*)contents dictId:(unsigned)dictId
{
    std::vector<size_t> contentSizes;
    NSMutableString* allContent = [[NSMutableString alloc] init];
    size_t totalSize = 0;
    static size_t g_maxSize = 4L * 1024 * 1024 * 1024;
    for (NSString* content in contents) {
        size_t size = [content lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        if (totalSize + size > g_maxSize) {
            break;
        }
        totalSize += size;
        [allContent appendString:content];
        contentSizes.push_back(size);
    }
    size_t bufferSize = self.dictSize;
    void* dictBuffer = malloc(bufferSize);
    XCTAssert(dictBuffer != nullptr);
    size_t dictSize = ZDICT_trainFromBuffer(dictBuffer, bufferSize, allContent.UTF8String, contentSizes.data(), (unsigned int) contentSizes.size());
    if (ZDICT_isError(dictSize)) {
        [self log:@"train dict error with code %llu, name %s", dictSize, ZDICT_getErrorName(dictSize)];
        return nil;
    }
    dictSize = ZDICT_finalizeDictionary(dictBuffer, bufferSize, dictBuffer, dictSize, allContent.UTF8String, contentSizes.data(), (unsigned int) contentSizes.size(), { self.compressLevel, 0, dictId });
    if (ZDICT_isError(dictSize)) {
        [self log:@"finalize dict error with code %llu, name %s", dictSize, ZDICT_getErrorName(dictSize)];
        return nil;
    }
    ZDict* dict = [[ZDict alloc] init];
    dict.dict = [[NSData alloc] initWithBytes:dictBuffer length:dictSize];
    dict.cDict = ZSTD_createCDict(dictBuffer, dictSize, self.compressLevel);
    dict.dDict = ZSTD_createDDict(dictBuffer, dictSize);
    return dict;
}

- (DictType)getDictType:(int)type content:(NSString*)content
{
    if (!self.usingTypedDict) {
        return DictType_msgCommon;
    }
    switch (type) {
    case MM_DATA_TEXT:
        return DictType_Text;
    case MM_DATA_IMG:
        return DictType_ImageMsg;
    case MM_DATA_VOICEMSG:
        return DictType_Voice;
    case MM_DATA_VIDEO:
        return DictType_Video;
    case MM_DATA_EMOJI:
        return DictType_Emoji;
    case MM_DATA_APPMSG: {
        if (!self.usingTypedDictForAppMsg) {
            return DictType_App;
        } else {
            NSRange typeStart = [content rangeOfString:@"<type>"];
            if (typeStart.location == NSNotFound) {
                return DictType_App;
            }
            NSRange typeEnd = [content rangeOfString:@"</type>" options:0 range:NSMakeRange(NSMaxRange(typeStart), content.length - NSMaxRange(typeStart))];
            if (typeEnd.location == NSNotFound) {
                return DictType_App;
            }
            NSString* typeString = [content substringWithRange:NSMakeRange(NSMaxRange(typeStart), typeEnd.location - NSMaxRange(typeStart))];
            int innerType = typeString.intValue;
            switch (innerType) {
            case MM_APP_URL:
                return DictType_Link;
            case MM_APP_RECORD:
                return DictType_Record;
            case MM_APP_MSG_REFER:
                return DictType_Ref;
            case MM_APP_FILE:
                return DictType_File;
            case MM_APP_APPBRAND:
                return DictType_WeApp;
            case MM_APP_FINDER_FEED:
                return DictType_Finder;
            case MM_APP_OPENSDK_APPBRAND:
                return DictType_OpenSDKWeApp;
            case MM_APP_NOTE:
                return DictType_Note;
            }
        }
    }
    case MM_DATA_SYSCMD_NEWXML:
        return DictType_NEWXML;
    default:
        return DictType_OtherMsg;
    }
}

- (WCDB::Expression)getMsgFilterConditionForDictType:(DictType)type
{
    WCDB::Expression condition = DBMessage.status.in({ 2, 3, 4 }) && DBMessage.des.in({ 0, 1 });
    switch (type) {
    case DictType_Text:
        return condition && DBMessage.type == MM_DATA_TEXT;
    case DictType_ImageMsg:
        return condition && DBMessage.type == MM_DATA_IMG;
    case DictType_Voice:
        return condition && DBMessage.type == MM_DATA_VOICEMSG;
    case DictType_Video:
        return condition && DBMessage.type == MM_DATA_VIDEO;
    case DictType_Emoji:
        return condition && DBMessage.type == MM_DATA_EMOJI;
    case DictType_App:
        return condition && DBMessage.type == MM_DATA_APPMSG;
    case DictType_NEWXML:
        return condition && DBMessage.type == MM_DATA_SYSCMD_NEWXML;
    case DictType_OtherMsg:
        return condition && DBMessage.type.notIn({ MM_DATA_TEXT, MM_DATA_IMG, MM_DATA_VOICEMSG, MM_DATA_VIDEO, MM_DATA_EMOJI, MM_DATA_APPMSG, MM_DATA_SYS, MM_DATA_SYSCMD_NEWXML });
    case DictType_Link:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>5</type>%");
    case DictType_Record:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>19</type>%");
    case DictType_Ref:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>57</type>%");
    case DictType_File:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>6</type>%");
    case DictType_WeApp:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>33</type>%");
    case DictType_Finder:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>51</type>%");
    case DictType_OpenSDKWeApp:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>36</type>%");
    case DictType_Note:
        return condition && DBMessage.type == MM_DATA_APPMSG && DBMessage.message.like("%<type>24</type>%");
    default:
        return WCDB::Expression();
    }
}

- (bool)trainDict
{
    NSMutableDictionary<NSNumber*, NSMutableArray<NSString*>*>* trainingContents = [[NSMutableDictionary alloc] init];
    for (int i = 0; i < DictType_Count; i++) {
        trainingContents[@(i)] = [[NSMutableArray alloc] init];
    }
    if (!self.usingRandomDict) {
        if (!self.usingCommonDict) {
            [self findTrainingMessage:trainingContents inTables:self.allTables from:self.database];
        } else {
            bool isBrand = [self.path containsString:@"BrandMsg"];
            for (int pathIndex = (isBrand ? 0 : 3); pathIndex < (isBrand ? 3 : self.allDatabasePaths.count); pathIndex++) {
                WCTDatabase* database = [[WCTDatabase alloc] initWithPath:self.allDatabasePaths[pathIndex]];
                NSArray* tables = [self.database getColumnOnResultColumn:WCTMaster.name
                                                               fromTable:WCTMaster.tableName
                                                                   where:WCTMaster.type == "table"
                                                                         && (WCTMaster.name.like("Chat\\_%%").escape("\\") || WCTMaster.name.like("ChatExt2\\_%%").escape("\\"))];
                if (![self findTrainingMessage:trainingContents inTables:tables from:database]) {
                    break;
                }
            }
        }
    } else {
        NSMutableArray* contents = [[NSMutableArray alloc] init];
        for (int i = 0; i < self.dictMsgCount; i++) {
            [contents addObject:[Random.shared stringWithLength:Random.shared.uint16 % 5000]];
        }
        trainingContents[@(DictType_msgCommon)] = contents;
        trainingContents[@(DictType_msgExt)] = contents;
    }

    __block bool succeed = true;
    [trainingContents enumerateKeysAndObjectsUsingBlock:^(NSNumber* type,
                                                          NSMutableArray<NSString*>* contents,
                                                          BOOL* stop) {
        if (contents.count > 0) {
            if (contents.count < self.dictMsgCount) {
                [self log:@"Require %d msg of type %@, but only has %llu", self.dictMsgCount, type, contents.count];
            }
            ZDict* dict = [self tranDictWith:contents dictId:type.unsignedIntValue];
            if (dict == nil) {
                [self log:@"train %@ dict fail with content %@", type, contents];
                *stop = true;
                succeed = false;
                return;
            }
            dict.trainCount = (int) contents.count;
            self.dicts[type] = dict;
        }
    }];
    return succeed;
}

- (bool)findTrainingMessage:(NSMutableDictionary<NSNumber*, NSMutableArray<NSString*>*>*)trainingContents inTables:(NSArray<NSString*>*)tables from:(WCTDatabase*)database
{
    bool needMore = false;
    for (NSString* table in tables) {
        if ([self isExtTable:table]) {
            if (trainingContents[@(DictType_msgExt)].count >= self.dictMsgCount) {
                continue;
            }
            NSArray<DBMessageExt*>* allExtMsg = [self.database getObjectsOfClass:DBMessageExt.class fromTable:table limit:self.dictMsgCount - trainingContents[@(DictType_msgExt)].count];
            for (DBMessageExt* msg in allExtMsg) {
                if (msg.msgSource.length == 0) {
                    continue;
                }
                [trainingContents[@(DictType_msgExt)] addObject:msg.msgSource];
            }
            needMore = YES;
        } else {
            if (!self.usingTypedDict) {
                if (trainingContents[@(DictType_msgCommon)].count >= self.dictMsgCount) {
                    continue;
                }
                needMore = YES;
                NSArray<DBMessage*>* allMsg = [self.database getObjectsOfClass:DBMessage.class fromTable:table limit:self.dictMsgCount - trainingContents[@(DictType_msgCommon)].count];
                for (DBMessage* msg in allMsg) {
                    if (msg.message.length == 0) {
                        continue;
                    }
                    [trainingContents[@(DictType_msgCommon)] addObject:msg.message];
                }
            } else {
                for (int i = DictType_Text; i < (self.usingTypedDictForAppMsg ? DictType_Count : DictType_Link); i++) {
                    if (trainingContents[@(i)].count >= self.dictMsgCount) {
                        continue;
                    }
                    if (self.usingTypedDictForAppMsg && i == DictType_App) {
                        continue;
                    }
                    needMore = YES;
                    NSArray<DBMessage*>* allMsg = [self.database getObjectsOfClass:DBMessage.class fromTable:table where:[self getMsgFilterConditionForDictType:(DictType) i] limit:self.dictMsgCount - trainingContents[@(i)].count];
                    for (DBMessage* msg in allMsg) {
                        if (msg.message.length == 0) {
                            continue;
                        }
                        [trainingContents[@(i)] addObject:msg.message];
                    }
                }
            }
        }
    }
    return needMore;
}

- (NSData*)compressString:(NSString*)content usingDict:(ZDict*)dict needReport:(bool)needReport
{
    static void* buffer = nullptr;
    static size_t bufferSize = 0;
    const char* src = content.UTF8String;
    size_t srcLength = src != nil ? strlen(src) : 0;
    if (srcLength > 0) {
        clock_t start = clock();
        size_t boundSize = ZSTD_compressBound(srcLength);
        XCTAssertFalse(ZSTD_isError(boundSize));
        if (boundSize > bufferSize) {
            if (buffer != nullptr) {
                free(buffer);
            }
            buffer = malloc(boundSize);
            bufferSize = boundSize;
            XCTAssertNotEqual(buffer, nullptr);
        }
        size_t compressedSize = 0;
        if (dict.cDict) {
            compressedSize = ZSTD_compress_usingCDict(_cctx, buffer, bufferSize, src, srcLength, dict.cDict);
        } else {
            compressedSize = ZSTD_compress2(_cctx, buffer, bufferSize, src, srcLength);
        }
        XCTAssertFalse(ZSTD_isError(compressedSize));
        if (compressedSize >= srcLength) {
            return [content dataUsingEncoding:NSUTF8StringEncoding];
        }
        if (needReport) {
            self.compressCount++;
            self.compressTime += clock() - start;
        }
        dict.compressCount++;
        dict.preLength += srcLength;
        dict.compressLength += compressedSize;
        return [[NSData alloc] initWithBytes:buffer length:compressedSize];
    } else {
        return [content dataUsingEncoding:NSUTF8StringEncoding];
    }
}

- (NSString*)decompressData:(NSData*)data usingDict:(ZDict*)dict needReport:(bool)needReport
{
    static void* buffer = malloc(1000 * 1024);
    static size_t bufferSize = 1000 * 1024;
    XCTAssertTrue(buffer != nullptr);
    size_t decompressSize = 0;
    clock_t start = clock();
    if (!dict.dDict) {
        decompressSize = ZSTD_decompressDCtx(_dctx, buffer, bufferSize, data.bytes, data.length);
    } else {
        decompressSize = ZSTD_decompress_usingDDict(_dctx, buffer, bufferSize, data.bytes, data.length, dict.dDict);
    }
    if (!ZSTD_isError(decompressSize)) {
        if (needReport && decompressSize > data.length) {
            self.decompressCount++;
            self.decompressTime += clock() - start;
        }
        return [[NSString alloc] initWithBytes:buffer length:decompressSize encoding:NSUTF8StringEncoding];
    } else {
        return [[NSString alloc] initWithBytes:data.bytes length:data.length encoding:NSUTF8StringEncoding];
    }
}

- (void)compressMsg
{
    WCTHandle* handle = [self.compressedDatabase getHandle];
    XCTAssertNotNil(handle);

    double progress = 0;
    double increment = 1.0 / self.allTables.count;
    double preProgress = 0;
    int64_t msgCount = 0;
    [self log:@"compressing database with db %@ id %@", self.currentBenchmark.compressDB, self.currentBenchmark.compressId];
    for (NSString* table in self.allTables) {
        @autoreleasepool {
            if ([self isExtTable:table]) {
                NSArray<DBMessageExt*>* allMsg = [self.database getObjectsOfClass:DBMessageExt.class fromTable:table];
                XCTAssertTrue([self.compressedDatabase createTable:table withClass:DBMessageExtBlob.class]);
                auto insert = WCDB::StatementInsert().insertIntoTable(table).columns(DBMessageExtBlob.allProperties).values(WCDB::BindParameter::bindParameters(DBMessageExtBlob.allProperties.size()));
                WCTPreparedStatement* insertSTMT = [handle getOrCreatePreparedStatement:insert];
                XCTAssertNotNil(insertSTMT);
                for (DBMessageExt* msg in allMsg) {
                    @autoreleasepool {
                        [insertSTMT reset];
                        DBMessageExtBlob* msgBlob = [[DBMessageExtBlob alloc] initFromDBMessageExt:msg];
                        msgBlob.msgSource = [self compressString:msg.msgSource usingDict:self.dicts[@(DictType_msgExt)] needReport:false];
                        [insertSTMT bindProperties:DBMessageExtBlob.allProperties ofObject:msgBlob];
                        XCTAssertTrue([insertSTMT step]);
                    }
                    msgCount++;
                    if (msgCount > 1000) {
                        [self.compressedDatabase passiveCheckpoint];
                        msgCount = 0;
                    }
                }
            } else {
                NSArray<DBMessage*>* allMsg = [self.database getObjectsOfClass:DBMessage.class fromTable:table];
                XCTAssertTrue([self.compressedDatabase createTable:table withClass:DBMessageBlob.class]);
                auto insert = WCDB::StatementInsert().insertIntoTable(table).columns(DBMessageBlob.allProperties).values(WCDB::BindParameter::bindParameters(DBMessageBlob.allProperties.size()));
                WCTPreparedStatement* insertSTMT = [handle getOrCreatePreparedStatement:insert];
                XCTAssertNotNil(insertSTMT);
                for (DBMessage* msg in allMsg) {
                    @autoreleasepool {
                        [insertSTMT reset];
                        DBMessageBlob* msgBlob = [[DBMessageBlob alloc] initFromDBMessage:msg];
                        DictType dictType = [self getDictType:msgBlob.type content:msg.message];
                        msgBlob.message = [self compressString:msg.message usingDict:self.dicts[@(dictType)] needReport:true];
                        [insertSTMT bindProperties:DBMessageBlob.allProperties ofObject:msgBlob];
                        XCTAssertTrue([insertSTMT step]);
                    }
                    msgCount++;
                    if (msgCount > 1000) {
                        [self.compressedDatabase passiveCheckpoint];
                        msgCount = 0;
                    }
                }
            }
        }
        progress += increment;
        if (progress - preProgress >= 0.1) {
            [self log:@"Progress: %0.2f", progress];
            preProgress = progress;
        }
    }
    [handle invalidate];
    [self.compressedDatabase truncateCheckpoint];
}

- (void)decompressMsg
{
    int remainMsgCount = 10000;
    for (NSString* table in self.allTables.reversedArray) {
        if ([self isExtTable:table]) {
            continue;
        } else {
            NSArray<DBMessageBlob*>* allMsgs = [self.compressedDatabase getObjectsOfClass:DBMessageBlob.class fromTable:table limit:remainMsgCount];
            for (DBMessageBlob* msgBlob in allMsgs) {
                DictType dictType = [self getDictType:msgBlob.type content:nil];
                NSString* message = [self decompressData:msgBlob.message usingDict:self.dicts[@(dictType)] needReport:true];
                XCTAssertNotNil(message);
            }
            remainMsgCount -= allMsgs.count;
        }
        if (remainMsgCount <= 0) {
            break;
        }
    }
}

- (void)resetStatus
{
    self.compressTime = 0;
    self.compressCount = 0;
    self.decompressTime = 0;
    self.decompressCount = 0;
}

- (void)testDictConfig
{
    self.compressLevel = ZSTD_defaultCLevel();
    self.skipDebugLog = YES;
    for (NSString* path in self.allDatabasePaths) {
        self.allTables = nil;
        for (NSNumber* dictSize in @[ @(10 * 1024), @(100 * 1024), @(1024 * 1024) ]) {
            self.dictSize = dictSize.intValue;
            for (NSNumber* dictMsgCount in @[ @1000, @10000, @100000 ]) {
                @autoreleasepool {
                    [self.dicts removeAllObjects];
                    self.dictMsgCount = dictMsgCount.intValue;
                    [self commonTestWithPath:path andId:[NSString stringWithFormat:@"%d-%d-%d", self.usingCommonDict, self.dictSize, self.dictMsgCount]];
                }
            }
        }
    }
    [self printBenchmark];
}

- (void)testCompressLevelConfig
{
    self.skipDebugLog = YES;
    bool preIsBrand = false;
    for (NSString* path in self.allDatabasePaths) {
        [self setPath:path];
        self.allTables = nil;
        self.dictSize = 100 * 1024;
        self.dictMsgCount = 10000;
        for (NSNumber* compressLevel in @[ @-9, @-6, @-3, @0, @3, @6, @9, @12, @15, @18, @21 ]) {
            @autoreleasepool {
                if (preIsBrand != [path containsString:@"BrandMsg"]) {
                    [self.dicts removeAllObjects];
                }
                self.allTables = nil;
                self.compressLevel = compressLevel.intValue;
                [self commonTestWithPath:path andId:[NSString stringWithFormat:@"%d-%d-%d-%d", self.usingCommonDict, self.dictSize, self.dictMsgCount, self.compressLevel]];
            }
        }
        preIsBrand = [path containsString:@"BrandMsg"];
    }
    [self printBenchmark];
}

- (void)testRandomDict
{
    self.compressLevel = ZSTD_defaultCLevel();
    self.skipDebugLog = YES;
    self.dictSize = 100 * 1024;
    self.dictMsgCount = 10000;

    self.usingCommonDict = YES;
    [self.dicts removeAllObjects];
    self.dicts[@(DictType_Count)] = [[ZDict alloc] init];
    for (NSString* path in self.allDatabasePaths) {
        @autoreleasepool {
            self.allTables = nil;
            [self commonTestWithPath:path andId:@"NoDict"];
        }
    }

    [self.dicts removeAllObjects];
    self.usingRandomDict = YES;

    for (NSString* path in self.allDatabasePaths) {
        @autoreleasepool {
            self.allTables = nil;
            [self.dicts removeAllObjects];
            [self commonTestWithPath:path andId:@"Random"];
        }
    }
    [self printBenchmark];
}

- (void)testDictCompressRadio
{
    self.compressLevel = ZSTD_defaultCLevel();
    self.skipDebugLog = YES;
    self.dictSize = 100 * 1024;
    self.dictMsgCount = 10000;
    self.usingCommonDict = YES;
    self.usingTypedDict = YES;

    bool hasLogTitle = false;
    for (NSString* path in self.allDatabasePaths) {
        @autoreleasepool {
            if ([path containsString:@"BrandMsg"]) {
                continue;
            }
            self.allTables = nil;
            [self commonTestWithPath:path andId:@"UnTypedAppMsg"];
            [self printDictCompressRadio:!hasLogTitle];
            hasLogTitle = true;
        }
    }

    [self.dicts removeAllObjects];
    self.usingTypedDictForAppMsg = YES;
    hasLogTitle = false;

    for (NSString* path in self.allDatabasePaths) {
        @autoreleasepool {
            if ([path containsString:@"BrandMsg"]) {
                continue;
            }
            self.allTables = nil;
            [self commonTestWithPath:path andId:@"TypedAppMsg"];
            [self printDictCompressRadio:!hasLogTitle];
            hasLogTitle = true;
        }
    }
}

- (void)testNormalCompressRadio
{
    self.compressLevel = ZSTD_defaultCLevel();
    self.skipDebugLog = YES;
    self.dictSize = 100 * 1024;
    self.dictMsgCount = 10000;
    self.usingCommonDict = YES;
    self.usingTypedDict = YES;
    for (int type = 0; type < DictType_Count; type++) {
        self.dicts[@(type)] = [[ZDict alloc] init];
    }

    bool hasLogTitle = false;
    for (NSString* path in self.allDatabasePaths) {
        @autoreleasepool {
            if ([path containsString:@"BrandMsg"]) {
                continue;
            }
            self.allTables = nil;
            [self commonTestWithPath:path andId:@"UnTypedAppMsg"];
            [self printDictCompressRadio:!hasLogTitle];
            hasLogTitle = true;
        }
    }

    [self.dicts removeAllObjects];
    self.usingTypedDictForAppMsg = YES;
    hasLogTitle = false;

    for (NSString* path in self.allDatabasePaths) {
        @autoreleasepool {
            if ([path containsString:@"BrandMsg"]) {
                continue;
            }
            self.allTables = nil;
            [self commonTestWithPath:path andId:@"TypedAppMsg"];
            [self printDictCompressRadio:!hasLogTitle];
            hasLogTitle = true;
        }
    }
}

- (void)printDictCompressRadio:(bool)printTitle
{
    NSMutableString* log = [[NSMutableString alloc] initWithString:[self getCompressDBIdFrom:self.path]];
    [log appendFormat:@"\t%f\t", self.currentBenchmark.compressRadio];
    NSMutableString* title = [[NSMutableString alloc] initWithString:@"DictType: \ttotal\t"];
    for (int type = 0; type < DictType_Count; type++) {
        ZDict* dict = self.dicts[@(type)];
        if (dict == nil) {
            continue;
        }
        if (printTitle) {
            [title appendFormat:@"\t%d\t", type];
        }
        [log appendFormat:@"\t%d\t%f", dict.compressCount, (double) dict.compressLength / dict.preLength];
        dict.compressCount = 0;
        dict.compressLength = 0;
        dict.preLength = 0;
    }
    if (printTitle) {
        [self log:title];
    }
    [self log:log];
}

- (void)testCommonDict
{
    self.compressLevel = ZSTD_defaultCLevel();
    self.skipDebugLog = YES;
    bool preIsBrand = false;

    for (NSNumber* usingTypedDict in @[ @NO, @YES ]) {
        for (NSNumber* useCommonDict in @[ @NO, @YES ]) {
            for (NSNumber* dictMsgCount in @[ @1000, @10000, @100000 ]) {
                self.usingTypedDict = usingTypedDict.boolValue;
                self.usingCommonDict = useCommonDict.boolValue;
                self.dictSize = 100 * 1024;
                self.dictMsgCount = dictMsgCount.intValue;
                [self.dicts removeAllObjects];
                for (NSString* path in self.allDatabasePaths) {
                    if (self.usingTypedDict && [path containsString:@"BrandMsg"]) {
                        continue;
                    }
                    @autoreleasepool {
                        self.allTables = nil;
                        if (preIsBrand != [path containsString:@"BrandMsg"]) {
                            [self.dicts removeAllObjects];
                        }
                        [self commonTestWithPath:path andId:[NSString stringWithFormat:@"%d-%d-%d", self.usingTypedDict, self.usingCommonDict, self.dictMsgCount]];
                        preIsBrand = [path containsString:@"BrandMsg"];
                    }
                }
            }
        }
    }
    [self printBenchmark];
}

- (NSString*)getCompressDBIdFrom:(NSString*)path
{
    NSArray* components = [path pathComponents];
    NSString* compressDB = [components[components.count - 2] stringByAppendingString:components[components.count - 1]];
    return compressDB;
}

- (void)commonTestWithPath:(NSString*)path andId:(NSString*)testId
{
    [self setPath:path];
    if (self.allTables.count == 0) {
        self.allTables = [self.database getColumnOnResultColumn:WCTMaster.name
                                                      fromTable:WCTMaster.tableName
                                                          where:WCTMaster.type == "table"
                                                                && (WCTMaster.name.like("Chat\\_%%").escape("\\") || WCTMaster.name.like("ChatExt2\\_%%").escape("\\"))];
    }
    NSString* compressDB = [self getCompressDBIdFrom:path];

    self.compressedDatabase = [[WCTDatabase alloc] initWithPath:[self getCompressDatabasePath:path]];
    [self.compressedDatabase enableAutoCheckpoint:NO];
    if (self.benchmarks[path] == nil) {
        self.benchmarks[path] = [[NSMutableArray alloc] init];
    }

    [self setupCtx];
    [self.compressedDatabase removeFiles];
    [self resetStatus];
    if (!self.usingCommonDict || self.dicts.count == 0) {
        if (![self trainDict]) {
            return;
        }
    }
    self.currentBenchmark = [[CompressBenchmark alloc] init];
    self.currentBenchmark.compressDB = compressDB;
    self.currentBenchmark.compressId = testId;
    [self.benchmarks[path] addObject:self.currentBenchmark];

    [self compressMsg];
    self.currentBenchmark.compressTimeInUs = self.compressCount > 0 ? self.compressTime / self.compressCount : 0;
    self.currentBenchmark.compressRadio = (double) [self.compressedDatabase getFilesSize] / [self.database getFilesSize];

    if (!self.usingTypedDictForAppMsg) {
        [self decompressMsg];
        self.currentBenchmark.decompressTimeInUs = self.decompressCount > 0 ? self.decompressTime / self.decompressCount : 0;
    }
}

- (void)printBenchmark
{
    NSMutableString* title = [[NSMutableString alloc] initWithString:@"Title:"];
    for (CompressBenchmark* benchmark in self.benchmarks.allValues.firstObject) {
        [title appendFormat:@"\t%@", benchmark.compressId];
    }
    [self log:@"Compress radio"];
    [self log:title];
    for (NSString* path in self.allDatabasePaths) {
        NSArray<CompressBenchmark*>* benchmarks = self.benchmarks[path];
        if (benchmarks == nil) {
            continue;
        }
        NSMutableString* log = [[NSMutableString alloc] initWithString:benchmarks.firstObject.compressDB];
        for (CompressBenchmark* benchmark in benchmarks) {
            [log appendFormat:@"\t%.4f", benchmark.compressRadio];
        }
        [self log:log];
    }

    [self log:@"Compress time"];
    [self log:title];
    for (NSString* path in self.allDatabasePaths) {
        NSArray<CompressBenchmark*>* benchmarks = self.benchmarks[path];
        if (benchmarks == nil) {
            continue;
        }
        NSMutableString* log = [[NSMutableString alloc] initWithString:benchmarks.firstObject.compressDB];
        for (CompressBenchmark* benchmark in benchmarks) {
            [log appendFormat:@"\t%lld", benchmark.compressTimeInUs];
        }
        [self log:log];
    }

    [self log:@"Decompress time"];
    [self log:title];
    for (NSString* path in self.allDatabasePaths) {
        NSArray<CompressBenchmark*>* benchmarks = self.benchmarks[path];
        if (benchmarks == nil) {
            continue;
        }
        NSMutableString* log = [[NSMutableString alloc] initWithString:benchmarks.firstObject.compressDB];

        for (CompressBenchmark* benchmark in benchmarks) {
            [log appendFormat:@"\t%lld", benchmark.decompressTimeInUs];
        }
        [self log:log];
    }
}

- (void)testVacuumAllDatabase
{
    for (NSString* path in self.allDatabasePaths) {
        [self setPath:path];
        XCTAssertTrue([self.database getFilesSize] > 0);
        [self.database enableAutoCheckpoint:NO];
        XCTAssertTrue([self.database execute:WCDB::StatementVacuum().vacuum()]);
    }
}

- (void)testCheckpointAllDatabase
{
    for (NSString* path in self.allDatabasePaths) {
        [self setPath:path];
        XCTAssertTrue([self.database getFilesSize] > 0);
        [self.database enableAutoCheckpoint:NO];
        XCTAssertTrue([self.database truncateCheckpoint]);
    }
}

- (void)testGetAllType
{
    int* msgCount = (int*) calloc(10003, sizeof(int));
    int64_t* msgLength = (int64_t*) calloc(10003, sizeof(int64_t));
    int* appMsgCount = (int*) calloc(10000, sizeof(int));
    int64_t* appMsgLength = (int64_t*) calloc(10000, sizeof(int64_t));
    for (NSString* path in self.allDatabasePaths) {
        if ([path containsString:@"BrandMsg"]) {
            continue;
        }
        [self setPath:path];
        self.allTables = [self.database getColumnOnResultColumn:WCTMaster.name
                                                      fromTable:WCTMaster.tableName
                                                          where:WCTMaster.type == "table"
                                                                && (WCTMaster.name.like("Chat\\_%%").escape("\\") || WCTMaster.name.like("ChatExt2\\_%%").escape("\\"))];
        for (NSString* table in self.allTables) {
            @autoreleasepool {
                if ([table hasPrefix:@"ChatExt2"]) {
                    continue;
                }
                WCTColumnsXRows* rows = [self.database getRowsFromStatement:WCDB::StatementSelect().select({ DBMessage.type, WCDB::Column().all().count() }).from(table).group(DBMessage.type)];
                for (WCTOneRow* row in rows) {
                    msgCount[row[0].numberValue.intValue] += row[1].numberValue.intValue;
                }
                rows = [self.database getRowsFromStatement:WCDB::StatementSelect().select({ DBMessage.type, DBMessage.message.length().sum() }).from(table).group(DBMessage.type)];
                for (WCTOneRow* row in rows) {
                    msgLength[row[0].numberValue.intValue] += row[1].numberValue.intValue;
                }
                NSArray<DBMessage*>* appMsgs = [self.database getObjectsOfClass:DBMessage.class fromTable:table where:[self getMsgFilterConditionForDictType:DictType_App]];
                for (DBMessage* msg in appMsgs) {
                    @autoreleasepool {
                        NSRange typeStart = [msg.message rangeOfString:@"<type>"];
                        if (typeStart.location == NSNotFound) {
                            continue;
                        }
                        NSRange typeEnd = [msg.message rangeOfString:@"</type>" options:0 range:NSMakeRange(NSMaxRange(typeStart), msg.message.length - NSMaxRange(typeStart))];
                        if (typeEnd.location == NSNotFound) {
                            continue;
                        }
                        NSString* typeString = [msg.message substringWithRange:NSMakeRange(NSMaxRange(typeStart), typeEnd.location - NSMaxRange(typeStart))];
                        int type = typeString.intValue;
                        if (type < 10000) {
                            appMsgCount[type]++;
                            appMsgLength[type] += [msg.message lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
                        } else {
                            type = 0;
                        }
                    }
                }
            }
        }
    }
    [self log:@"print msg:"];
    for (int i = 1; i < 10003; i++) {
        if (msgCount[i] > 0) {
            [self log:@"Type Info:\t%d\t%d\t%lld", i, msgCount[i], msgLength[i]];
        }
    }
    [self log:@"print app msg:"];
    for (int i = 1; i < 10000; i++) {
        if (appMsgCount[i] > 0) {
            [self log:@"Type Info:\t%d\t%d\t%lld", i, appMsgCount[i], appMsgLength[i]];
        }
    }
}

- (void)testDirectCompress
{
    self.skipDebugLog = YES;
    NSData* brandDictData = [NSData dataWithContentsOfFile:[self.desktop stringByAppendingPathComponent:@"ZSTDTest/BrandDict.zstd"]];
    TestCaseAssertTrue([WCTDatabase registerZSTDDict:brandDictData andDictId:1]);
    NSData* brandExtDictData = [NSData dataWithContentsOfFile:[self.desktop stringByAppendingPathComponent:@"ZSTDTest/BrandExtDict.zstd"]];
    TestCaseAssertTrue([WCTDatabase registerZSTDDict:brandExtDictData andDictId:2]);

    int i = 0;
    for (NSString* path in self.allDatabasePaths) {
        if (![path containsString:@"BrandMsg"]) {
            continue;
        }
        i++;

        NSString* testPath = [path stringByAppendingString:@"-test"];
        WCTDatabase* database = [[WCTDatabase alloc] initWithPath:testPath];
        [database removeFiles];
        [self.fileManager copyItemsIfExistsAtPath:path toPath:testPath];

        [database setCompressionWithFilter:^(WCTCompressionUserInfo* info) {
            if ([info.table hasPrefix:@"Chat_"]) {
                [info addZSTDDictCompressProperty:DBMessage.message withDictId:1];
            } else if ([info.table hasPrefix:@"ChatExt2_"]) {
                [info addZSTDDictCompressProperty:DBMessageExt.msgSource withDictId:2];
            }
        }];
        int totalPageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::pageCount())].numberValue.intValue;
        int freePageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount())].numberValue.intValue;
        int preCount = totalPageCount - freePageCount;
        [self log:@"Database %d page count before compression: %d, free rate %f", i, preCount, (double) freePageCount / totalPageCount];
        while (!database.isCompressed) {
            [database stepCompression];
        }
        totalPageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::pageCount())].numberValue.intValue;
        freePageCount = [database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount())].numberValue.intValue;
        int afterCount = totalPageCount - freePageCount;
        [self log:@"Database %d page count after compression: %d, compression ratio %f", i, afterCount, (double) afterCount / preCount];
    }
}

@end
