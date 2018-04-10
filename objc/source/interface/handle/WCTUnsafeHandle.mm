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
#import <WCDB/NSData+noCopyData.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>
#import <WCDB/WCTValue+Private.h>

@implementation WCTUnsafeHandle

#pragma mark - Initialize
//Safe
//- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database;

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
             andRecyclableHandle:(const WCDB::RecyclableHandle &)recyclableHandle
{
    //Safe
    if (recyclableHandle == nullptr) {
        return nil;
    }
    if (self = [super initWithDatabase:database]) {
        _recyclableHandle = recyclableHandle;
        _handle = recyclableHandle.getHandle();
    }
    return self;
}

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
                       andHandle:(WCDB::Handle *)handle
{
    //Unsafe
    assert(handle != nullptr);
    if (self = [super initWithDatabase:database]) {
        _handle = handle;
    }
    return self;
}

- (void)dealloc
{
    [self finalizeHandle];
}

#pragma mark - Safety

- (BOOL)isSafe
{
    //It's safe if it will or did capture a recyclable one
    return _recyclableHandle != nullptr || _handle == nullptr;
}

- (void)generateIfNeeded
{
    if (_handle == nullptr) {
        _recyclableHandle = _database->flowOut();
        if (_recyclableHandle != nullptr) {
            _handle = _recyclableHandle.getHandle();
        } else {
            _nonHandleError = [WCTError errorWithWCDBError:_database->getError()];
        }
    }
}

- (WCDB::Handle *)getOrGenerateHandle
{
    [self generateIfNeeded];
    return _handle;
}

- (void)finalizeHandleIfGeneratedAndKeepError:(BOOL)keepError
{
    if (_handle) {
        _handle->finalize();
    }
    if (_recyclableHandle != nullptr) {
        if (keepError) {
            _nonHandleError = [WCTError errorWithWCDBError:_handle->getError()];
        } else {
            _nonHandleError = nil;
        }
        _handle = nullptr;
        _recyclableHandle = nullptr;
    }
}

#pragma mark - Life Cycle

- (void)finalizeStatement
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    if (_handle) {
        _handle->finalize();
    }
}

- (void)finalizeHandle
{
    [self finalizeHandleIfGeneratedAndKeepError:NO];
}

#pragma mark - Bind row
- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
    const std::shared_ptr<WCTBaseAccessor> &accessor = columnBinding->accessor;
    switch (accessor->getAccessorType()) {
        case WCTAccessorCpp: {
            switch (accessor->getColumnType()) {
                case WCDB::ColumnType::Integer32: {
                    WCTCppAccessor<WCDB::ColumnType::Integer32> *i32Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer32> *) accessor.get();
                    _handle->bindInteger32(i32Accessor->getValue(object),
                                           index);
                } break;
                case WCDB::ColumnType::Integer64: {
                    WCTCppAccessor<WCDB::ColumnType::Integer64> *i64Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer64> *) accessor.get();
                    _handle->bindInteger64(i64Accessor->getValue(object),
                                           index);
                } break;
                case WCDB::ColumnType::Float: {
                    WCTCppAccessor<WCDB::ColumnType::Float> *floatAccessor = (WCTCppAccessor<WCDB::ColumnType::Float> *) accessor.get();
                    _handle->bindDouble(floatAccessor->getValue(object),
                                        index);
                } break;
                case WCDB::ColumnType::Text: {
                    WCTCppAccessor<WCDB::ColumnType::Text> *textAccessor = (WCTCppAccessor<WCDB::ColumnType::Text> *) accessor.get();
                    _handle->bindText(textAccessor->getValue(object),
                                      index);
                } break;
                case WCDB::ColumnType::BLOB: {
                    WCTCppAccessor<WCDB::ColumnType::BLOB> *blobAccessor = (WCTCppAccessor<WCDB::ColumnType::BLOB> *) accessor.get();
                    _handle->bindBLOB(blobAccessor->getValue(object), index);
                } break;
                case WCDB::ColumnType::Null:
                    _handle->bindNull(index);
                    break;
            }
        } break;
        case WCTAccessorObjC: {
            WCTObjCAccessor *objcAccessor = (WCTObjCAccessor *) accessor.get();
            NSObject *value = objcAccessor->getObject(object);
            if (value) {
                switch (accessor->getColumnType()) {
                    case WCDB::ColumnType::Integer32: {
                        NSNumber *number = (NSNumber *) value;
                        _handle->bindInteger32(number.intValue, index);
                        break;
                    }
                    case WCDB::ColumnType::Integer64: {
                        NSNumber *number = (NSNumber *) value;
                        _handle->bindInteger64(number.longLongValue, index);
                        break;
                    }
                    case WCDB::ColumnType::Float: {
                        NSNumber *number = (NSNumber *) value;
                        _handle->bindDouble(number.doubleValue, index);
                        break;
                    }
                    case WCDB::ColumnType::Text: {
                        NSString *string = (NSString *) value;
                        _handle->bindText(string.UTF8String, (int) string.length, index);
                        break;
                    }
                    case WCDB::ColumnType::BLOB: {
                        NSData *data = (NSData *) value;
                        _handle->bindBLOB(data.noCopyData, index);
                        break;
                    }
                    case WCDB::ColumnType::Null:
                        _handle->bindNull(index);
                        break;
                }
            } else {
                _handle->bindNull(index);
            }
        } break;
    }
}

- (void)bindProperties:(const WCTPropertyList &)properties
              ofObject:(WCTObject *)object
{
    int i = 0;
    for (const WCTProperty &property : properties) {
        ++i;
        [self bindProperty:property
                  ofObject:object
                   toIndex:i];
    }
}

- (void)bindValue:(WCTColumnCodingValue *)value
          toIndex:(int)index
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    value = [value archivedWCTValue];
    if ([value isKindOfClass:NSData.class]) {
        NSData *data = (NSData *) value;
        _handle->bindBLOB(data.noCopyData, index);
    } else if ([value isKindOfClass:NSString.class]) {
        NSString *string = (NSString *) value;
        _handle->bindText(string.UTF8String, (int) string.length, index);
    } else if ([value isKindOfClass:NSNumber.class]) {
        NSNumber *number = (NSNumber *) value;
        if (CFNumberIsFloatType((CFNumberRef) number)) {
            _handle->bindDouble(number.doubleValue, index);
        } else {
            if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
                _handle->bindInteger32(number.intValue, index);
            } else {
                _handle->bindInteger64(number.longLongValue, index);
            }
        }
    } else {
        _handle->bindNull(index);
    }
}

#pragma mark - Get row
- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(WCTObject *)object
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
    const std::shared_ptr<WCTBaseAccessor> &accessor = columnBinding->accessor;
    switch (accessor->getAccessorType()) {
        case WCTAccessorCpp: {
            switch (accessor->getColumnType()) {
                case WCDB::ColumnType::Integer32: {
                    WCTCppAccessor<WCDB::ColumnType::Integer32> *i32Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer32> *) accessor.get();
                    i32Accessor->setValue(object, _handle->getInteger32(index));
                } break;
                case WCDB::ColumnType::Integer64: {
                    WCTCppAccessor<WCDB::ColumnType::Integer64> *i64Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer64> *) accessor.get();
                    i64Accessor->setValue(object, _handle->getInteger64(index));
                } break;
                case WCDB::ColumnType::Float: {
                    WCTCppAccessor<WCDB::ColumnType::Float> *floatAccessor = (WCTCppAccessor<WCDB::ColumnType::Float> *) accessor.get();
                    floatAccessor->setValue(object, _handle->getDouble(index));
                } break;
                case WCDB::ColumnType::Text: {
                    WCTCppAccessor<WCDB::ColumnType::Text> *textAccessor = (WCTCppAccessor<WCDB::ColumnType::Text> *) accessor.get();
                    textAccessor->setValue(object, _handle->getText(index));
                } break;
                case WCDB::ColumnType::BLOB: {
                    WCTCppAccessor<WCDB::ColumnType::BLOB> *blobAccessor = (WCTCppAccessor<WCDB::ColumnType::BLOB> *) accessor.get();
                    blobAccessor->setValue(object, _handle->getBLOB(index));
                } break;
                case WCDB::ColumnType::Null: {
                    WCTCppAccessor<WCDB::ColumnType::Null> *nullAccessor = (WCTCppAccessor<WCDB::ColumnType::Null> *) accessor.get();
                    nullAccessor->setValue(object, nullptr);
                } break;
            }
        } break;
        case WCTAccessorObjC: {
            WCTObjCAccessor *objcAccessor = (WCTObjCAccessor *) accessor.get();
            id value = nil;
            if (_handle->getType(index) != WCDB::ColumnType::Null) {
                switch (accessor->getColumnType()) {
                    case WCDB::ColumnType::Integer32:
                        value = [NSNumber numberWithInt:_handle->getInteger32(index)];
                        break;
                    case WCDB::ColumnType::Integer64:
                        value = [NSNumber numberWithLongLong:_handle->getInteger64(index)];
                        break;
                    case WCDB::ColumnType::Float:
                        value = [NSNumber numberWithDouble:_handle->getDouble(index)];
                        break;
                    case WCDB::ColumnType::Text:
                        value = [NSString stringWithUTF8String:_handle->getText(index)];
                        break;
                    case WCDB::ColumnType::BLOB:
                        value = [NSData dataWithNoCopyData:_handle->getBLOB(index)];
                        break;
                    case WCDB::ColumnType::Null:
                        value = nil;
                        break;
                }
            }
            objcAccessor->setObject(object, value);
        } break;
    }
}

- (WCTValue *)getValueAtIndex:(int)index
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    switch (_handle->getType(index)) {
        case WCDB::ColumnType::Integer32:
            return [[WCTValue alloc] initWithInteger32:_handle->getInteger32(index)];
        case WCDB::ColumnType::Integer64:
            return [[WCTValue alloc] initWithInteger64:_handle->getInteger64(index)];
        case WCDB::ColumnType::Float:
            return [[WCTValue alloc] initWithDouble:_handle->getDouble(index)];
        case WCDB::ColumnType::Text:
            return [[WCTValue alloc] initWithString:_handle->getText(index)];
        case WCDB::ColumnType::BLOB:
            return [[WCTValue alloc] initWithNoCopyData:_handle->getBLOB(index)];
        case WCDB::ColumnType::Null:
            return [[WCTValue alloc] initWithNull];
    }
}

- (WCTOneRow *)getRow
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    NSMutableArray<WCTValue *> *row = [[NSMutableArray<WCTValue *> alloc] init];
    for (int i = 0; i < _handle->getColumnCount(); ++i) {
        [row addObject:[self getValueAtIndex:i]];
    }
    return row;
}

- (WCTObject *)getObjectOfClass:(Class)cls onProperties:(const WCTPropertyList &)properties
{
    WCTObject *object = [[cls alloc] init];
    if (!object) {
        return nil;
    }
    int index = 0;
    for (const WCTProperty &property : properties) {
        [self extractValueAtIndex:index
                       toProperty:property
                         ofObject:object];
        ++index;
    };
    return object;
}

#pragma mark - Error

- (NSError *)getError
{
    if (_handle) {
        return [WCTError errorWithWCDBError:_handle->getError()];
    }
    return _nonHandleError;
}

#pragma mark - Convenient
- (BOOL)execute:(const WCDB::Statement &)statement
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->execute(statement);
}

- (BOOL)prepare:(const WCDB::Statement &)statement
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->prepare(statement);
}

- (WCTValue *)nextValueAtIndex:(int)index orDone:(BOOL &)isDone
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    if (_handle->step((bool &) isDone) && !isDone) {
        return [self getValueAtIndex:index];
    }
    [self finalizeStatement];
    return nil;
}

- (WCTOneColumn *)allValuesAtIndex:(int)index
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    NSMutableArray<WCTValue *> *values = [[NSMutableArray<WCTValue *> alloc] init];
    bool done;
    while (_handle->step(done) && !done) {
        [values addObject:[self getValueAtIndex:index]];
    }
    [self finalizeStatement];
    return done ? values : nil;
}

- (WCTOneRow *)nextRowOrDone:(BOOL &)isDone
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    if (_handle->step((bool &) isDone) && !isDone) {
        return [self getRow];
    }
    [self finalizeStatement];
    return nil;
}

- (WCTColumnsXRows *)allRows
{
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    NSMutableArray<WCTOneRow *> *rows = [[NSMutableArray<WCTOneRow *> alloc] init];
    bool done;
    while (_handle->step(done) && !done) {
        [rows addObject:[self getRow]];
    }
    [self finalizeStatement];
    return done ? rows : nil;
}

- (id /* WCTObject* */)nextObjectOfClass:(Class)cls orDone:(BOOL &)isDone
{
    return [self nextObjectOfClass:cls onProperties:[cls objectRelationalMappingForWCDB]->getAllProperties() orDone:isDone];
}

- (id /* WCTObject* */)nextObjectOnProperties:(const WCTPropertyList &)properties orDone:(BOOL &)isDone
{
    if (properties.empty()) {
        return nil;
    }
    Class cls = properties.front().getColumnBinding()->getClass();
    return [self nextObjectOfClass:cls onProperties:properties orDone:isDone];
}

- (id /* WCTObject* */)nextObjectOfClass:(Class)cls onProperties:(const WCTPropertyList &)properties orDone:(BOOL)isDone
{
    NSAssert([cls conformsToProtocol:@protocol(WCTTableCoding)], @"%@ should conforms to WCTTableCoding protocol", cls);
    if (_handle->step((bool &) isDone) && !isDone) {
        return [self getObjectOfClass:cls onProperties:properties];
    }
    [self finalizeStatement];
    return nil;
}

- (NSArray /* <WCTObject*> */ *)allObjectsOfClass:(Class)cls
{
    return [self allObjectsOfClass:cls onProperties:[cls objectRelationalMappingForWCDB]->getAllProperties()];
}

- (NSArray /* <WCTObject*> */ *)allObjectsOnProperties:(const WCTPropertyList &)properties
{
    if (properties.empty()) {
        return nil;
    }
    Class cls = properties.front().getColumnBinding()->getClass();
    return [self allObjectsOfClass:cls onProperties:properties];
}

- (NSArray /* <WCTObject*> */ *)allObjectsOfClass:(Class)cls onProperties:(const WCTPropertyList &)properties
{
    NSAssert([cls conformsToProtocol:@protocol(WCTTableCoding)], @"%@ should conforms to WCTTableCoding protocol", cls);
    NSAssert(_handle != nullptr, @"[prepare] or [execute] should be called before this.");
    NSMutableArray<WCTObject *> *objects = [[NSMutableArray<WCTObject *> alloc] init];
    bool done;
    while (_handle->step(done) && !done) {
        [objects addObject:[self getObjectOfClass:cls onProperties:properties]];
    }
    [self finalizeStatement];
    return done ? objects : nil;
}

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object
{
    Class cls = object.class;
    NSAssert([cls conformsToProtocol:@protocol(WCTTableCoding)], @"%@ should conforms to WCTTableCoding protocol", cls);
    const WCTPropertyList &properties = [cls objectRelationalMappingForWCDB]->getAllProperties();
    return [self execute:statement
              withObject:object
            onProperties:properties];
}

- (BOOL)execute:(const WCDB::Statement &)statement
      withObject:(WCTObject *)object
    onProperties:(const WCTPropertyList &)properties
{
    if (![self prepare:statement]) {
        return NO;
    }
    [self bindProperties:properties ofObject:object];
    BOOL result = _handle->step();
    [self finalizeStatement];
    return result;
}

- (BOOL)execute:(const WCDB::Statement &)statement
      withValue:(WCTColumnCodingValue *)value
{
    if (![self prepare:statement]) {
        return NO;
    }
    [self bindValue:value toIndex:1];
    BOOL result = _handle->step();
    [self finalizeStatement];
    return result;
}

- (BOOL)execute:(const WCDB::Statement &)statement
        withRow:(WCTOneRow *)row
{
    if (![self prepare:statement]) {
        return NO;
    }
    int index = 1;
    for (WCTColumnCodingValue *value in row) {
        [self bindValue:value toIndex:index];
        ++index;
    }
    BOOL result = _handle->step();
    [self finalizeStatement];
    return result;
}

- (BOOL)rebindTable:(NSString *)tableName toClass:(Class<WCTTableCoding>)cls
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    std::string table = tableName.UTF8String;
    const WCTBinding *binding = [cls objectRelationalMappingForWCDB];
    std::pair<bool, bool> isTableExists = handle->isTableExists(table);
    if (!isTableExists.first) {
        return NO;
    }
    if (isTableExists.second) {
        auto pair = handle->getColumnsWithTable(table);
        if (!pair.first) {
            return NO;
        }
        const std::list<std::string> &columnNames = pair.second;
        //Check whether the column names exists
        std::map<std::string, std::shared_ptr<WCTColumnBinding>, WCDB::String::CaseInsensiveComparator> columnBindingMap = binding->getColumnBindingMap();
        for (const std::string &columnName : columnNames) {
            auto iter = columnBindingMap.find(columnName);
            if (iter == columnBindingMap.end()) {
                WCDB::Error::warning([NSString stringWithFormat:@"Skip column named [%s] for table [%s]", columnName.c_str(), tableName.UTF8String].UTF8String);
            } else {
                columnBindingMap.erase(iter);
            }
        }
        //Add new column
        for (const auto &iter : columnBindingMap) {
            if (!handle->execute(WCDB::StatementAlterTable().alterTable(table).addColumn(iter.second->columnDef))) {
                return NO;
            }
        }
    } else {
        if (!handle->execute(binding->generateCreateTableStatement(tableName.UTF8String))) {
            return NO;
        }
    }
    for (const WCDB::StatementCreateIndex &statementCreateIndex : binding->generateCreateIndexStatements(table)) {
        if (!handle->execute(statementCreateIndex)) {
            return NO;
        }
    }
    return YES;
}

- (void)finalizeDatabase
{
    [self finalizeDatabase:NO];
}

- (instancetype)autoFinalizeStatement
{
    _finalizeLevel = WCTFinalizeLevelStatement;
    return self;
}

- (instancetype)autoFinalizeHandle
{
    _finalizeLevel = WCTFinalizeLevelHandle;
    return self;
}

- (instancetype)autoFinalizeDatabase
{
    _finalizeLevel = WCTFinalizeLevelDatabase;
    return self;
}

- (void)finalizeDatabase:(BOOL)keepError
{
    [self finalizeHandleIfGeneratedAndKeepError:keepError];
    [super finalizeDatabase];
}

- (void)doAutoFinalize:(BOOL)keepError
{
    switch (_finalizeLevel) {
        case WCTFinalizeLevelHandle:
            [self finalizeHandleIfGeneratedAndKeepError:keepError];
            break;
        case WCTFinalizeLevelStatement:
            [self finalizeStatement];
            break;
        case WCTFinalizeLevelDatabase:
            [self finalizeDatabase:keepError];
            break;
        default:
            break;
    }
}

@end
