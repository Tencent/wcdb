//
//  HandlePoolHolder.hpp
//  WCDB
//
//  Created by sanhuazhang on 2018/6/12.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

#ifndef HandlePoolHolder_hpp
#define HandlePoolHolder_hpp

#include <WCDB/HandlePoolRelated.hpp>
#include <WCDB/RecyclableHandlePool.hpp>

namespace WCDB {

class HandlePoolHolder : public HandlePoolRelated {
public:
    HandlePoolHolder(const RecyclableHandlePool &pool);

private:
    RecyclableHandlePool m_holdingPool;
};

} //namespace WCDB

#endif /* HandlePoolHolder_hpp */
