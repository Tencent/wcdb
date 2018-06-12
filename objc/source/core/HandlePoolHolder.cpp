//
//  HandlePoolHolder.cpp
//  WCDB
//
//  Created by sanhuazhang on 2018/6/12.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

#include <WCDB/HandlePoolHolder.hpp>

namespace WCDB {

HandlePoolHolder::HandlePoolHolder(const RecyclableHandlePool &pool)
    : HandlePoolRelated(pool.getHandlePool()), m_holdingPool(pool)
{
}

} //namespace WCDB
