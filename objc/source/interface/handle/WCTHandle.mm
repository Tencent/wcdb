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

#import <WCDB/Assertion.hpp>
#import <WCDB/Interface.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTHandle

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
{
    if (self = [super initWithDatabase:database]) {
        _finalizeLevel = WCTFinalizeLevelStatement;
    }
    return self;
}

- (WCTDatabase *)getDatabase
{
    return [[WCTDatabase alloc] initWithDatabase:_database];
}

#pragma mark - Info
- (long long)getLastInsertedRowID
{
    WCTHandleAssert(return 0;);
    return _handle->getLastInsertedRowID();
}

- (int)getChanges
{
    WCTHandleAssert(return 0;);
    return _handle->getChanges();
}

- (BOOL)isStatementReadonly
{
    WCTHandleAssert(return NO;);
    return _handle->isStatementReadonly();
}

#pragma mark - Statement
- (BOOL)prepare:(const WCDB::Statement &)statement
{
    return [super prepare:statement];
}

- (void)finalizeStatement
{
    [super finalizeStatement];
}

#pragma mark - Stepping
- (BOOL)step:(BOOL &)done
{
    WCTHandleAssert(return NO;);
    return _handle->step((bool &) done);
}

- (BOOL)step
{
    WCTHandleAssert(return NO;);
    return _handle->step();
}

- (void)reset
{
    WCTHandleAssert(return;);
    _handle->reset();
}

#pragma mark - Bind row
- (void)bindValue:(WCTColumnCodingValue *)value toIndex:(int)index
{
    [super bindValue:value toIndex:index];
}

- (void)bindBool:(BOOL)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindInteger32(value ? 1 : 0, index);
}

- (void)bindInteger32:(const int32_t &)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindInteger32(value, index);
}

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindInteger64(value, index);
}

- (void)bindDouble:(const double &)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindDouble(value, index);
}

- (void)bindString:(NSString *)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindText(value.UTF8String, index);
}

- (void)bindBLOB:(NSData *)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindBLOB(WCDB::Data::immutableNoCopyData((const unsigned char *) value.bytes, (size_t) value.length), index);
}

- (void)bindNullToIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindNull(index);
}

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index
{
    [super bindProperty:property
               ofObject:object
                toIndex:index];
}

- (void)bindProperties:(const WCTPropertyList &)properties
              ofObject:(WCTObject *)object
{
    [super bindProperties:properties ofObject:object];
}

#pragma mark - Get row
- (WCTValue *)getValueAtIndex:(int)index
{
    return [super getValueAtIndex:index];
}

- (WCTOneRow *)getRow
{
    return [super getRow];
}

- (BOOL)getBoolAtIndex:(int)index
{
    WCTHandleAssert(return NO;);
    return _handle->getInteger32(index) != 0;
}

- (int32_t)getInteger32AtIndex:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getInteger32(index);
}

- (int64_t)getInteger64AtIndex:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getInteger64(index);
}

- (double)getDouble:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getDouble(index);
}

- (NSString *)getTextAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithUTF8String:_handle->getText(index)];
}

- (NSData *)getBLOB:(int)index
{
    WCTHandleAssert(return nil;);
    const WCDB::Data data = _handle->getBLOB(index);
    return [NSData dataWithBytes:data.buffer() length:data.size()];
}

- (WCDB::ColumnType)getColumnTypeAtIndex:(int)index
{
    WCTHandleAssert(return WCDB::ColumnType::Null;);
    return _handle->getType(index);
}

- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(WCTObject *)object
{
    [super extractValueAtIndex:index
                    toProperty:property
                      ofObject:object];
}

- (WCTObject *)getObjectOfClass:(Class)cls
{
    const WCTPropertyList &properties = [cls allProperties];
    return [self getObjectOfClass:cls onProperties:properties];
}

- (WCTObject *)getObjectOnProperties:(const WCTPropertyList &)properties
{
    Class cls = properties.front().getColumnBinding().getClass();
    return [self getObjectOfClass:cls onProperties:properties];
}

- (int)getColumnCount
{
    WCTHandleAssert(return 0;);
    return _handle->getColumnCount();
}

- (NSString *)getColumnNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    const char *columnName = _handle->getColumnName(index);
    if (columnName) {
        return [NSString stringWithUTF8String:columnName];
    }
    return nil;
}

- (NSString *)getColumnTableNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    const char *tableName = _handle->getColumnTableName(index);
    if (tableName) {
        return [NSString stringWithUTF8String:tableName];
    }
    return nil;
}

#pragma mark - Execute
- (BOOL)execute:(const WCDB::Statement &)statement
{
    return [super execute:statement];
}

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object
{
    return [super execute:statement withObject:object];
}

- (BOOL)execute:(const WCDB::Statement &)statement
      withObject:(WCTObject *)object
    onProperties:(const WCTPropertyList &)properties
{
    return [super execute:statement withObject:object onProperties:properties];
}

- (BOOL)execute:(const WCDB::Statement &)statement
      withValue:(WCTColumnCodingValue *)value
{
    return [super execute:statement withValue:value];
}

- (BOOL)execute:(const WCDB::Statement &)statement
        withRow:(WCTOneRow *)row
{
    return [super execute:statement withRow:row];
}

@end
