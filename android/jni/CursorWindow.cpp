/*
 * Copyright (C) 2006-2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#undef LOG_TAG
#define LOG_TAG "WCDB.CursorWindow"

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "CursorWindow.h"

#include <string>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>


namespace wcdb {

    CursorWindow::CursorWindow(void* data, size_t size) :
        mData(data), mSize(size) {
        mHeader = static_cast<Header*>(mData);
    }

    CursorWindow::~CursorWindow() {
        if (mData)
            free(mData);
    }

    status_t CursorWindow::create(size_t size, CursorWindow** outCursorWindow) {
        status_t result;
        void* data = malloc(size);
        if (data > 0) {
            CursorWindow* window = new CursorWindow(data, size);
            result = window->clear();
            if (!result) {
                LOG_WINDOW("Created new CursorWindow: freeOffset=%" PRIu32 ", "
                    "numRows=%" PRIu32 ", numColumns=%" PRIu32 ", mSize=%zu, mData=%p",
                    window->mHeader->freeOffset,
                    window->mHeader->numRows,
                    window->mHeader->numColumns,
                    window->mSize, window->mData);
                *outCursorWindow = window;
            }
            return OK;
        } else {
            *outCursorWindow = NULL;
            return NO_MEMORY;
        }
    }

    status_t CursorWindow::clear() {
        mHeader->freeOffset = sizeof(Header) + sizeof(RowSlotChunk);
        mHeader->firstChunkOffset = sizeof(Header);
        mHeader->numRows = 0;
        mHeader->numColumns = 0;

        RowSlotChunk* firstChunk = static_cast<RowSlotChunk*>(offsetToPtr(mHeader->firstChunkOffset));
        firstChunk->nextChunkOffset = 0;
        return OK;
    }

    status_t CursorWindow::setNumColumns(uint32_t numColumns) {
        uint32_t cur = mHeader->numColumns;
        if ((cur > 0 || mHeader->numRows > 0) && cur != numColumns) {
            ALOGE("Trying to go from %d columns to %d", cur, numColumns);
            return INVALID_OPERATION;
        }
        mHeader->numColumns = numColumns;
        return OK;
    }

    status_t CursorWindow::allocRow(RowSlot **outSlot) {
        // Fill in the row slot
        RowSlot* rowSlot = allocRowSlot();
        if (rowSlot == NULL) {
            if (outSlot) *outSlot = NULL;
            return NO_MEMORY;
        }

        // Allocate the slots for the field directory
        size_t fieldDirSize = mHeader->numColumns * sizeof(FieldSlot);
        uint32_t fieldDirOffset = alloc(fieldDirSize, true /*aligned*/);
        if (!fieldDirOffset) {
            mHeader->numRows--;
            LOG_WINDOW("The row failed, so back out the new row accounting "
                "from allocRowSlot %d", mHeader->numRows);
            if (outSlot) *outSlot = NULL;
            return NO_MEMORY;
        }
        FieldSlot* fieldDir = static_cast<FieldSlot*>(offsetToPtr(fieldDirOffset));
        memset(fieldDir, 0, fieldDirSize);

        LOG_WINDOW("Allocated row %" PRIu32 ", rowSlot is at offset %" PRIu32 
            ", fieldDir is %zu bytes at offset %" PRIu32 "\n",
            mHeader->numRows - 1, offsetFromPtr(rowSlot), fieldDirSize, fieldDirOffset);
        rowSlot->offset = fieldDirOffset;

        if (outSlot) *outSlot = rowSlot;
        return OK;
    }

    status_t CursorWindow::freeLastRow() {
        if (mHeader->numRows > 0) {
            mHeader->numRows--;
        }
        return OK;
    }

    uint32_t CursorWindow::alloc(size_t size, bool aligned) {
        uint32_t padding;
        if (aligned) {
            // 4 byte alignment
            padding = (~mHeader->freeOffset + 1) & 3;
        }
        else {
            padding = 0;
        }

        uint32_t offset = mHeader->freeOffset + padding;
        uint32_t nextFreeOffset = offset + size;
        if (nextFreeOffset > mSize) {
            ALOGW("Window is full: requested allocation %zu bytes, "
                "free space %zu bytes, window size %zu bytes",
                size, freeSpace(), mSize);
            return 0;
        }

        mHeader->freeOffset = nextFreeOffset;
        return offset;
    }

    CursorWindow::RowSlot* CursorWindow::getRowSlot(uint32_t row) {
        if (row >= mHeader->numRows) {
            ALOGE("Failed to read row %d from a CursorWindow which has %d rows.",
                row, mHeader->numRows);
            return NULL;
        }
        
        uint32_t chunkPos = row;
        RowSlotChunk* chunk = static_cast<RowSlotChunk*>(
            offsetToPtr(mHeader->firstChunkOffset));
        while (chunkPos >= ROW_SLOT_CHUNK_NUM_ROWS) {
            chunk = static_cast<RowSlotChunk*>(offsetToPtr(chunk->nextChunkOffset));
            chunkPos -= ROW_SLOT_CHUNK_NUM_ROWS;
        }
        return &chunk->slots[chunkPos];
    }

    CursorWindow::RowSlot* CursorWindow::allocRowSlot() {
        uint32_t chunkPos = mHeader->numRows;
        RowSlotChunk* chunk = static_cast<RowSlotChunk*>(
            offsetToPtr(mHeader->firstChunkOffset));
        while (chunkPos > ROW_SLOT_CHUNK_NUM_ROWS) {
            chunk = static_cast<RowSlotChunk*>(offsetToPtr(chunk->nextChunkOffset));
            chunkPos -= ROW_SLOT_CHUNK_NUM_ROWS;
        }
        if (chunkPos == ROW_SLOT_CHUNK_NUM_ROWS) {
            if (!chunk->nextChunkOffset) {
                chunk->nextChunkOffset = alloc(sizeof(RowSlotChunk), true /*aligned*/);
                if (!chunk->nextChunkOffset) {
                    return NULL;
                }
            }
            chunk = static_cast<RowSlotChunk*>(offsetToPtr(chunk->nextChunkOffset));
            chunk->nextChunkOffset = 0;
            chunkPos = 0;
        }
        mHeader->numRows += 1;
        return &chunk->slots[chunkPos];
    }

    CursorWindow::FieldSlot* CursorWindow::getFieldSlot(RowSlot *rowSlot, uint32_t column) {
        if (!rowSlot || column >= mHeader->numColumns) {
            ALOGE("Failed to find row or column(%d) from a CursorWindow (has %d columns).",
                column, mHeader->numColumns);
            return NULL;
        }
        FieldSlot* fieldDir = static_cast<FieldSlot*>(offsetToPtr(rowSlot->offset));
        return &fieldDir[column];
    }

    status_t CursorWindow::putBlob(RowSlot *row, uint32_t column, const void* value, size_t size) {
        return putBlobOrString(row, column, value, size, FIELD_TYPE_BLOB);
    }

    status_t CursorWindow::putString(RowSlot *row, uint32_t column, const char* value, size_t sizeIncludingNull) {
        return putBlobOrString(row, column, value, sizeIncludingNull, FIELD_TYPE_STRING);
    }

    status_t CursorWindow::putBlobOrString(RowSlot *row, uint32_t column,
        const void* value, size_t size, int32_t type) {

        FieldSlot* fieldSlot = getFieldSlot(row, column);
        if (!fieldSlot) {
            return BAD_VALUE;
        }

        uint32_t offset = alloc(size);
        if (!offset) {
            return NO_MEMORY;
        }

        memcpy(offsetToPtr(offset), value, size);

        fieldSlot->type = type;
        fieldSlot->data.buffer.offset = offset;
        fieldSlot->data.buffer.size = size;
        return OK;
    }

    status_t CursorWindow::putLong(RowSlot *row, uint32_t column, int64_t value) {
        FieldSlot* fieldSlot = getFieldSlot(row, column);
        if (!fieldSlot) {
            return BAD_VALUE;
        }

        fieldSlot->type = FIELD_TYPE_INTEGER;
        fieldSlot->data.l = value;
        return OK;
    }

    status_t CursorWindow::putDouble(RowSlot *row, uint32_t column, double value) {
        FieldSlot* fieldSlot = getFieldSlot(row, column);
        if (!fieldSlot) {
            return BAD_VALUE;
        }

        fieldSlot->type = FIELD_TYPE_FLOAT;
        fieldSlot->data.d = value;
        return OK;
    }

    status_t CursorWindow::putNull(RowSlot *row, uint32_t column) {
        FieldSlot* fieldSlot = getFieldSlot(row, column);
        if (!fieldSlot) {
            return BAD_VALUE;
        }

        fieldSlot->type = FIELD_TYPE_NULL;
        fieldSlot->data.buffer.offset = 0;
        fieldSlot->data.buffer.size = 0;
        return OK;
    }

} // namespace wcdb
