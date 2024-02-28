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

#ifndef __WCDB_CHUNKED_CURSOR_WINDOW_H__
#define __WCDB_CHUNKED_CURSOR_WINDOW_H__

#include "CursorWindow.h"
#include "Mutex.h"
#include <map>

namespace wcdb {

class ChunkedCursorWindow {
    // Each CursorWindow chunk is 256 kB.
    static const uint32_t CHUNK_SIZE = 256 * 1024;

    struct Chunk {
        CursorWindow *window;
        uint32_t startPos;

        static Chunk *create(uint32_t startPos_, size_t size);

        void acquire();
        void release();

    private:
        Chunk(CursorWindow *window_, uint32_t startPos_);
        ~Chunk();
        volatile int mRefCount;
    };

public:
    class Row;
    class Field {
        Field(CursorWindow::FieldSlot *slot) : mSlot(slot) {}

        CursorWindow::FieldSlot *mSlot;

        friend class Row;
    };

    class Row {
        Row() : mPoolNext(nullptr) {}

        ~Row() {}

        uint32_t mRow;
        Chunk *mChunk;
        CursorWindow *mWindow;
        CursorWindow::RowSlot *mSlot;
        Row *mPoolNext;

        friend class ChunkedCursorWindow;

    public:
        inline uint32_t getRowPos() const { return mRow; }

        inline Field getField(uint32_t column)
        {
            return Field(mWindow->getFieldSlot(mSlot, column));
        }

        inline bool isFieldValid(const Field &field)
        {
            return field.mSlot != nullptr;
        }

        inline int32_t getFieldType(const Field &field)
        {
            return mWindow->getFieldSlotType(field.mSlot);
        }
        inline int64_t getFieldLong(const Field &field)
        {
            return mWindow->getFieldSlotValueLong(field.mSlot);
        }
        inline double getFieldDouble(const Field &field)
        {
            return mWindow->getFieldSlotValueDouble(field.mSlot);
        }
        inline const char *getFieldString(const Field &field,
                                          size_t &outSizeIncludingNull)
        {
            return mWindow->getFieldSlotValueString(field.mSlot,
                                                    &outSizeIncludingNull);
        }
        inline const void *getFieldBlob(const Field &field, size_t &outSize)
        {
            return mWindow->getFieldSlotValueBlob(field.mSlot, &outSize);
        }

        inline status_t putBlob(uint32_t column, const void *value, size_t size)
        {
            return mWindow->putBlob(mSlot, column, value, size);
        }
        inline status_t
        putString(uint32_t column, const char *value, size_t sizeIncludingNull)
        {
            return mWindow->putString(mSlot, column, value, sizeIncludingNull);
        }
        inline status_t putLong(uint32_t column, int64_t value)
        {
            return mWindow->putLong(mSlot, column, value);
        }
        inline status_t putDouble(uint32_t column, double value)
        {
            return mWindow->putDouble(mSlot, column, value);
        }
        inline status_t putNull(uint32_t column)
        {
            return mWindow->putNull(mSlot, column);
        }
    };

    ~ChunkedCursorWindow();

    static status_t create(size_t size, ChunkedCursorWindow **outWindow);

    inline uint32_t getNumColumns()
    {
        AutoMutex lock(mMutex);
        return mNumColumns;
    }
    status_t setNumColumns(uint32_t columns);

    Row *newRow(uint32_t row, bool newChunk);
    Row *getRow(uint32_t row);
    void endRow(Row *row);
    void rollbackRow(Row *row);
    status_t clear();
    status_t removeChunk(uint32_t row, uint32_t &outStart, uint32_t &outEnd);

    size_t capacity() const;
    void capacity(size_t size);

    inline size_t getNumChunks() const
    {
        AutoMutex lock(mMutex);
        return mChunkMap.size();
    }

private:
    ChunkedCursorWindow(uint32_t chunkCapacity);

    // startRow -> CursorWindow chunk mapping, used in row lookup.
    typedef std::map<uint32_t, Chunk *> ChunkMap;
    ChunkMap mChunkMap;
    uint32_t mChunkCapacity;

    // Mutex and condition to protect data and recieve notification
    // when data is ready for reading.
    mutable Mutex mMutex;

    uint32_t mNumColumns;

    // Last chunk accessed, cached for fast lookup of rows nearby.
    Chunk *mLastReadChunk;
    Chunk *mLastWriteChunk;
    uint32_t mLastWriteChunkRowLimit;
    uint32_t mCurrentWritingRow;

    Chunk *findChunkByRowLocked(uint32_t row);
    Chunk *getChunkForWritingLocked(uint32_t row);
    Chunk *allocChunkLocked(uint32_t startPos);
    Chunk *removeChunkLocked(uint32_t row);

    // Pool for Row objects to prevent frequent memory allocations.
    Row *mRowPool;
    Row *allocRowLocked();
    void recycleRowLocked(Row *row);
};

} // namespace wcdb

#endif
