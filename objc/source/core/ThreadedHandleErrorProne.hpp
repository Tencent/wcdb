//
//  ThreadedHandleErrorProne.hpp
//  WCDB
//
//  Created by sanhuazhang on 2018/5/21.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

#ifndef ThreadedHandleErrorProne_hpp
#define ThreadedHandleErrorProne_hpp

#include <WCDB/Error.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <unordered_map>

namespace WCDB {

class HandlePool;

class ThreadedHandleErrorProne {
protected:
    void setThreadedError(const Error &error) const;
    void setThreadedError(Error &&error) const;
    const Error &getThreadedError() const;
    virtual const HandlePool *getErrorAssociatedHandlePool() const = 0;
    void error(const std::string &message) const;

private:
    using ThreadedErrors = std::unordered_map<const HandlePool *, Error>;
    static ThreadLocal<ThreadedErrors> *threadedErrors();
};

} //namespace WCDB

#endif /* ThreadedHandleErrorProne_hpp */
