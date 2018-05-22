//
//  ThreadedHandleErrorProne.cpp
//  WCDB
//
//  Created by sanhuazhang on 2018/5/21.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

#include <WCDB/HandlePool.hpp>
#include <WCDB/ThreadedHandleErrorProne.hpp>

namespace WCDB {

ThreadLocal<ThreadedHandleErrorProne::ThreadedErrors> *
ThreadedHandleErrorProne::threadedErrors()
{
    static ThreadLocal<ThreadedErrors> s_threadedErrors;
    return &s_threadedErrors;
}

void ThreadedHandleErrorProne::setThreadedError(const Error &error) const
{
    (*threadedErrors()->get())[getErrorAssociatedHandlePool()] = error;
}

void ThreadedHandleErrorProne::setThreadedError(Error &&error) const
{
    (*threadedErrors()->get())[getErrorAssociatedHandlePool()] =
        std::move(error);
}

const Error &ThreadedHandleErrorProne::getThreadedError() const
{
    return (*threadedErrors()->get())[getErrorAssociatedHandlePool()];
}

void ThreadedHandleErrorProne::error(const std::string &message) const
{
    Error error(message);
    const HandlePool *handlePool = getErrorAssociatedHandlePool();
    if (handlePool->getTag() != Handle::invalidTag) {
        error.infos.set("Tag", handlePool->getTag());
    }
    error.infos.set("Path", handlePool->path);
    Reporter::shared()->report(error);
    (*threadedErrors()->get())[handlePool] = std::move(error);
}

} //namespace WCDB
