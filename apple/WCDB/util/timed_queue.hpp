//
//  mymap.hpp
//  WCDB
//
//  Created by sanhuazhang on 2017/6/28.
//  Copyright © 2017年 sanhuazhang. All rights reserved.
//

#ifndef mymap_hpp
#define mymap_hpp

#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <stdio.h>
#include <string>
#include <thread>
#include <unordered_map>

namespace WCDB {

template <typename Key>
class TimedQueue {
public:
    TimedQueue(int delay) : m_delay(std::chrono::seconds(delay)){};

    typedef std::function<void(const Key &)> OnExpired;

    void reQueue(const Key &key)
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        bool signal = m_list.empty();

        auto iter = m_map.find(key);
        if (iter != m_map.end()) {
            m_list.erase(iter->second);
            m_map.erase(iter);
        }

        //delay 2 seconds
        m_list.push_front({key, std::chrono::steady_clock::now() + m_delay});
        auto last = m_list.begin();
        m_map.insert({key, last});
        if (signal) {
            m_cond.notify_one();
        }
    }

    void waitUntilExpired(const OnExpired &onExpired, bool forever = true)
    {
        {
            std::unique_lock<std::mutex> lockGuard(m_mutex);
            while (m_list.empty()) {
                if (forever) {
                    m_cond.wait(lockGuard);
                } else {
                    return;
                }
            }
        }
        bool get = false;
        while (!get) {
            Element element;
            Time now = std::chrono::steady_clock::now();
            {
                std::unique_lock<std::mutex> lockGuard(m_mutex);
                element = m_list.back();
                if (now > element.second) {
                    m_list.pop_back();
                    m_map.erase(element.first);
                    get = true;
                }
            }
            if (get) {
                onExpired(element.first);
            } else {
                std::this_thread::sleep_for(element.second - now);
            }
        }
    }

protected:
    using Time = std::chrono::steady_clock::time_point;
    using Element = std::pair<Key, Time>;
    using List = std::list<Element>;
    using Map = std::unordered_map<Key, typename List::iterator>;
    Map m_map;
    List m_list;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    std::chrono::seconds m_delay;
};

} //namespace WCDB

#endif /* mymap_hpp */
