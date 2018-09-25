//
//  DebugDescribable.hpp
//  WCDB
//
//  Created by sanhuazhang(张三华) on 2018/9/25.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

#ifndef DebugDescribable_hpp
#define DebugDescribable_hpp

#include <string>

namespace WCDB {

class DebugDescribable {
public:
    virtual std::string getDebugDescription() const = 0;
};

} // namespace WCDB

#endif /* DebugDescribable_hpp */
