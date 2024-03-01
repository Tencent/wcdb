//
// Created by qiuwenchen on 2020/2/23.
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
#import "WCTCommon.h"
NS_ASSUME_NONNULL_BEGIN

/**
 This class is a wrapper for `sqlite3_stmt`.
 @warning Not Thread-safe.
 */
WCDB_API @interface WCTPreparedStatement : NSObject

/**
 You should use `-[WCTHandle getOrCreatePreparedStatement:]` to obtain WCTPreparedStatement.
 */
- (instancetype)init UNAVAILABLE_ATTRIBUTE;

#pragma mark - Step
/**
 @brief The wrapper of `sqlite3_step`.
 @return YES if no error occurs.
 */
- (BOOL)step;

/**
 @brief The wrapper of `sqlite3_reset`.
 */
- (void)reset;

/**
 @brief The wrapper of `sqlite3_clear_bindings`.
 */
- (void)clearBindings;

/**
 @brief Check if you can continue stepping.
 @return YES if you can continue stepping.
 */
- (BOOL)done;

#pragma mark - State

/**
 @brief The wrapper of `sqlite3_stmt_readonly`.
 */
- (BOOL)isStatementReadonly;

#pragma mark - Bind
/**
 @brief The wrapper of `sqlite3_bind_int64`.
 */
- (void)bindInteger:(const int64_t &)value toIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_bind_double`.
 */
- (void)bindDouble:(const double &)value toIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_bind_null`.
 */
- (void)bindNullToIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_bind_text`.
 */
- (void)bindString:(NSString *)string toIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_bind_int64`.
 */
- (void)bindData:(NSData *)data toIndex:(int)index;

/**
 @brief Bind an NSNumber.
 It will call `sqlite3_bind_int64` or `sqlite3_bind_double` according to the underlying data type of NSNumber.
 */
- (void)bindNumber:(NSNumber *)number toIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_bind_*`.
 It will call the appropriate routine according to the column type returned by `+[WCTColumnCoding columnType]`.
 @param value WCTColumnCodingValue can be NSString, NSNumber, NSData, NSNull, nil and any NSObject that conforms to WCTColumnCoding.
 @see   `WCTColumnCoding`
 */
- (void)bindValue:(nullable WCTColumnCodingValue *)value
          toIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_bind_*` for binding property of object to index.
 It will call the appropriate routine according to the data type of property.
 */
- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_bind_*` for binding the specified properties of object.
 It will call the appropriate routine according to the data type of properties.
 */
- (void)bindProperties:(const WCTProperties &)properties
              ofObject:(WCTObject *)object;

/**
 @brief The wrapper of `sqlite3_bind_parameter_index`.
 */
- (int)bindParameterIndex:(const WCDB::BindParameter &)parameter;

#pragma mark - Extract Column meta
/**
 @brief The wrapper of `sqlite3_column_count`.
 */
- (int)extractNumberOfColumns;

/**
 @brief The wrapper of `sqlite3_column_type`.
 */
- (WCTColumnType)extractTypeAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_origin_name`.
 */
- (NSString *)extractOriginColumnNameAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_name`.
 */
- (NSString *)extractColumnNameAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_table_name`.
 */
- (NSString *)extractTableNameAtIndex:(int)index;

#pragma mark - Extract Row data
/**
 @brief The wrapper of `sqlite3_column_int64`.
 */
- (int64_t)extractIntegerAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_double`.
 */
- (double)extractDoubleAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_text`.
 */
- (NSString *)extractStringAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_int64` and `sqlite3_column_double`.
 */
- (NSNumber *)extractNumberAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_blob`.
 */
- (NSData *)extractDataAtIndex:(int)index;

/**
 @brief The wrapper of `sqlite3_column_*`.
 @return The real type of WCTValue depends on the value in database. It could be NSNumber, NSString, NSData or nil.
 */
- (nullable WCTValue *)extractValueAtIndex:(int)index;

/**
 @brief Extract all values of current row.
 @return An array of NSObject that conforms to WCTValue.
 */
- (WCTOneRow *)extractRow;

/**
 @brief Extract the values of the current row and assign them into the properties specified by resultColumns of a new WCTTableCoding object.
 @return A WCTTableCoding object.
 */
- (WCTObject *)extractObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

/**
 @brief Extract the results of a multi-table query in the current row.
 @return A dictionary containing the result of multi-table. The keys of the dictionary are the table names, and their corresponding values are the results of these tables.
 */
- (WCTMultiObject *)extractMultiObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

/**
 @brief Extract value at index and set it to the corresponding property of object.
 */
- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(WCTObject *)object;

#pragma mark - Step And Extract All Data
/**
 @brief Extract all the values of the first column in the result.
 @return An array of NSObject that conforms to WCTValue, or nil if error occurs.
 */
- (nullable WCTOneColumn *)allValues;

/**
 @brief Extract all the values of the column at index in the result.
 @return An array of NSObject that conforms to WCTValue, or nil if error occurs.
 */
- (nullable WCTOneColumn *)allValuesAtIndex:(int)index;

/**
 @brief Extract all the values in the result.
 @return A two-dimensional array, or nil if error occurs.
 */
- (nullable WCTColumnsXRows *)allRows;

/**
 @brief Extract the values of all rows in the result and assign them into the properties specified by resultColumns of new WCTTableCoding objects.
 @return An array of WCTTableCoding object, or nil if error occurs.
 */
- (nullable NSArray /* <WCTObject*> */ *)allObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

/**
 @brief Extract the results of a multi-table query.
 @return An array of WCTMultiObject, or nil if error occurs.
 */
- (nullable NSArray<WCTMultiObject *> *)allMultiObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

@end
NS_ASSUME_NONNULL_END
