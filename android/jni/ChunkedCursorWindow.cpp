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

#include "ChunkedCursorWindow.h"
#include <assert.h>

namespace wcdb {

#define CHUNK_CONFLICT ((Chunk *) -1)
#define CHUNK_NOT_FOUND ((Chunk *) 0)

ChunkedCursorWindow::Chunk::Chunk(CursorWindow *window_, uint32_t startPos_)
    : window(window_), startPos(startPos_), mRefCount(1)
{
}

ChunkedCursorWindow::Chunk::~Chunk()
{
    delete window;
}

void ChunkedCursorWindow::Chunk::acquire()
{
    int ref = __sync_fetch_and_add(&mRefCount, 1);
    assert(ref > 0);
    (void) ref;
}

void ChunkedCursorWindow::Chunk::release()
{
    if (__sync_fetch_and_sub(&mRefCount, 1) == 1)
        delete this;
}

ChunkedCursorWindow::Chunk *
ChunkedCursorWindow::Chunk::create(uint32_t startPos_, size_t size)
{
    CursorWindow *window;
    status_t ret = CursorWindow::create(size, &window);
    if (ret == OK && window) {
        return new Chunk(window, startPos_);
    } else {
        return NULL;
    }
}

ChunkedCursorWindow::ChunkedCursorWindow(uint32_t chunkCapacity)
    : mChunkCapacity(chunkCapacity)
    , mNumColumns(0)
    , mLastReadChunk(NULL)
    , mLastWriteChunk(NULL)
    , mLastWriteChunkRowLimit(__UINT32_MAX__)
    , mCurrentWritingRow(__UINT32_MAX__)
    , mRowPool(NULL)
{
}

ChunkedCursorWindow::~ChunkedCursorWindow()
{
    // Release all allocated chunk.
    ChunkMap::iterator it;
    for (it = mChunkMap.begin(); it != mChunkMap.end(); ++it) {
        it->second->release();
    }

    // Free Row object pool.
    for (Row *p = mRowPool; p; p = p->mPoolNext) {
        delete p;
    }
}

status_t ChunkedCursorWindow::create(size_t size,
                                     ChunkedCursorWindow **outWindow)
{
    uint32_t capacity = size / CHUNK_SIZE;
    if (size % CHUNK_SIZE > 0)
        capacity++;

    *outWindow = new ChunkedCursorWindow(capacity);
    return OK;
}

ChunkedCursorWindow::Row *ChunkedCursorWindow::allocRowLocked()
{
    // Must be called with mMutex locked.

    Row *r;
    if (mRowPool) {
        r = mRowPool;
        mRowPool = r->mPoolNext;
        r->mPoolNext = NULL;
    } else {
        r = new Row();
    }
    return r;
}

void ChunkedCursorWindow::recycleRowLocked(ChunkedCursorWindow::Row *row)
{
    // Must be called with mMutex locked.

    assert(row->mPoolNext == NULL);
    row->mPoolNext = mRowPool;
    mRowPool = row;
}

size_t ChunkedCursorWindow::capacity() const
{
    AutoMutex lock(mMutex);
    return mChunkCapacity * CHUNK_SIZE;
}

void ChunkedCursorWindow::capacity(size_t size)
{
    AutoMutex lock(mMutex);

    uint32_t capacity = size / CHUNK_SIZE;
    if (size % CHUNK_SIZE > 0)
        capacity++;
    mChunkCapacity = capacity;
}

status_t ChunkedCursorWindow::setNumColumns(uint32_t columns)
{
    AutoMutex lock(mMutex);

    if (columns == mNumColumns)
        return OK;

    if (mNumColumns > 0 || !mChunkMap.empty())
        return INVALID_OPERATION;

    mNumColumns = columns;
    return OK;
}

ChunkedCursorWindow::Chunk *
ChunkedCursorWindow::findChunkByRowLocked(uint32_t row)
{
    // Must be called with mMutex locked.

    // Lookup mLastReadChunk first.
    if (mLastReadChunk &&
        row - mLastReadChunk->startPos < mLastReadChunk->window->getNumRows()) {
        return mLastReadChunk;
    }

    // Not found, search ChunkMap.
    Chunk *chunk = NULL;
    ChunkMap::iterator it = mChunkMap.lower_bound(row);
    if (it != mChunkMap.end() && it->first == row) {
        chunk = it->second;
    } else if (it != mChunkMap.begin()) {
        chunk = (--it)->second;
    }

    if (chunk) {
        if (row - chunk->startPos >= chunk->window->getNumRows())
            chunk = NULL;
        else
            mLastReadChunk = chunk;
    }

    return chunk;
}

ChunkedCursorWindow::Chunk *
ChunkedCursorWindow::getChunkForWritingLocked(uint32_t row)
{
    // Must be called with mMutex locked.

    // Lookup mLastWriteChunk first.
    if (mLastWriteChunk &&
        row - mLastWriteChunk->startPos ==
            mLastWriteChunk->window->getNumRows()) {
        if (row >= mLastWriteChunkRowLimit)
            return CHUNK_CONFLICT;
        else
            return mLastWriteChunk;
    }

    // By supporting only linear insertions, there are only two possible scenarios:
    // 1) append to last active chunk, which is handled above;
    // 2) allocate new chunk, which should call allocChunkLocked.
    // We don't even need to search ChunkMap here.
    return CHUNK_NOT_FOUND;
}

ChunkedCursorWindow::Chunk *
ChunkedCursorWindow::allocChunkLocked(uint32_t startPos)
{
    // TODO: check capacity.

    // Lookup if row to insert already existed.
    ChunkMap::iterator it = mChunkMap.lower_bound(startPos);
    ChunkMap::iterator hint = it;
    if (it != mChunkMap.end() && it->first == startPos) {
        // Found a chunk with exactly the same startPos.
        // Anyway it's not posible to insert a chunk here, just return failure.
        return NULL;
    } else if (it != mChunkMap.begin()) {
        Chunk *chunk = (--it)->second;

        // Got chunk just before the insertion point, check for conflict.
        if (startPos - chunk->startPos < chunk->window->getNumRows())
            return NULL;
    }
    // If we found no chunk with startPos lesser than insertion point, it's perfectly
    // okay to insert a chunk at the beginning.

    // Allocate new chunk and push to the map.
    Chunk *chunk = Chunk::create(startPos, CHUNK_SIZE);
    if (chunk->window->setNumColumns(mNumColumns) != OK) {
        chunk->release();
        return NULL;
    }
    mChunkMap.insert(hint, std::make_pair(startPos, chunk));

    // Update cache.
    mLastWriteChunk = chunk;
    mLastWriteChunkRowLimit =
        (hint == mChunkMap.end()) ? __UINT32_MAX__ : hint->second->startPos;
    return chunk;
}

ChunkedCursorWindow::Chunk *ChunkedCursorWindow::removeChunkLocked(uint32_t row)
{
    Chunk *chunk;
    ChunkMap::iterator it = mChunkMap.lower_bound(row);
    if (it != mChunkMap.end() && it->first == row) {
        chunk = it->second;
    } else if (it != mChunkMap.begin()) {
        chunk = (--it)->second;
        if (row - chunk->startPos < chunk->window->getNumRows())
            return NULL;
    } else { // it == mChunkMap.begin()
        return NULL;
    }

    if (mLastReadChunk == chunk)
        mLastReadChunk = NULL;
    if (mLastWriteChunk == chunk) {
        mLastWriteChunk = NULL;
        mLastWriteChunkRowLimit = __UINT32_MAX__;
    }

    mChunkMap.erase(it);
    return chunk;
}

ChunkedCursorWindow::Row *ChunkedCursorWindow::newRow(uint32_t row,
                                                      bool newChunk)
{
    AutoMutex lock(mMutex);

    if (mCurrentWritingRow != __UINT32_MAX__)
        return NULL;

    Chunk *chunk = NULL;
    if (!newChunk) {
        // Not forced to new chunk, lookup cache.
        chunk = getChunkForWritingLocked(row);

        if (chunk == CHUNK_CONFLICT) {
            // We are in conflict, remove the chunk in requesting position.
            Chunk *removedChunk = removeChunkLocked(row);
            assert(removedChunk != NULL);
            removedChunk->release();
            chunk = NULL;
        }
    }

    if (!chunk) {
        // No existing chunk, allocate a new one.
        chunk = allocChunkLocked(row);
        if (!chunk) {
            // Chunk allocation failed.
            return NULL;

            // TODO: handle capacity limit.
        }
    }

    // Allocate row on the chunk.
    assert(chunk->startPos + chunk->window->getNumRows() == row);

    CursorWindow *window = chunk->window;
    CursorWindow::RowSlot *slot;
    status_t ret = window->allocRow(&slot);
    if (ret != OK || !slot) {
        // Row allocation failed, probably because there is no space left on the window
        // to allocate RowSlotChunk or field directory. Return failure and let caller retry.
        return NULL;
    }

    // Fill Row object and return.
    chunk->acquire();
    Row *rowObj = allocRowLocked();
    rowObj->mRow = row;
    rowObj->mChunk = chunk;
    rowObj->mWindow = window;
    rowObj->mSlot = slot;

    mCurrentWritingRow = row;
    return rowObj;
}

ChunkedCursorWindow::Row *ChunkedCursorWindow::getRow(uint32_t row)
{
    AutoMutex lock(mMutex);

    if (mCurrentWritingRow == row)
        return NULL;

    Chunk *chunk = findChunkByRowLocked(row);
    if (!chunk)
        return NULL;

    assert(row >= chunk->startPos);
    CursorWindow *window = chunk->window;
    CursorWindow::RowSlot *slot = window->getRowSlot(row - chunk->startPos);
    if (!slot)
        return NULL;

    chunk->acquire();
    Row *rowObj = allocRowLocked();
    rowObj->mRow = row;
    rowObj->mChunk = chunk;
    rowObj->mWindow = window;
    rowObj->mSlot = slot;
    return rowObj;
}

void ChunkedCursorWindow::endRow(ChunkedCursorWindow::Row *row)
{
    AutoMutex lock(mMutex);

    if (mCurrentWritingRow == row->mRow)
        mCurrentWritingRow = __UINT32_MAX__;

    row->mChunk->release();
    recycleRowLocked(row);
}

void ChunkedCursorWindow::rollbackRow(ChunkedCursorWindow::Row *row)
{
    AutoMutex lock(mMutex);

    if (mCurrentWritingRow == row->mRow) {
        row->mWindow->freeLastRow();
        mCurrentWritingRow = __UINT32_MAX__;
    }

    row->mChunk->release();
    recycleRowLocked(row);
}

status_t ChunkedCursorWindow::clear()
{
    AutoMutex lock(mMutex);

    ChunkMap::iterator it;
    for (it = mChunkMap.begin(); it != mChunkMap.end(); ++it) {
        it->second->release();
    }
    mChunkMap.clear();

    mNumColumns = 0;
    mLastReadChunk = NULL;
    mLastWriteChunk = NULL;
    mLastWriteChunkRowLimit = __UINT32_MAX__;
    mCurrentWritingRow = __UINT32_MAX__;
    return OK;
}

status_t ChunkedCursorWindow::removeChunk(uint32_t row,
                                          uint32_t &outStart,
                                          uint32_t &outEnd)
{
    AutoMutex lock(mMutex);

    Chunk *chunk = removeChunkLocked(row);
    if (chunk) {
        outStart = chunk->startPos;
        outEnd = outStart + chunk->window->getNumRows();
        chunk->release();
    }

    return chunk ? OK : BAD_INDEX;
}

} // namespace wcdb
