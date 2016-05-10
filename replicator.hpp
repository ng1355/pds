//
//  replicator.hpp
//  backend
//
//  Created by Nikita Georgiou on 5/9/16.
//  Copyright © 2016 Nikita Georgiou. All rights reserved.
//

#ifndef replicator_hpp
#define replicator_hpp
#include <vector>
#include <utility>
#include "connection.h"
#include "fileManager.hpp"

namespace backend{
    class replicator{
    public:
        replicator(const char* port, std::vector<std::pair<std::string, bool> > ports){}
        unsigned multicast();
        bool compare(unsigned otherStamp);
        bool ping();
        bool pong();
        unsigned update(unsigned newStamp);
        void heartbeat();
    private:
        std::vector<std::pair<char*, bool> > ports;
        unsigned timestamp;
        connection conn;
        fileManager* fm;
    };
}



#endif /* replicator_hpp */
