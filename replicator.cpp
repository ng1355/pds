//
//  replicator.cpp
//  backend
//
//  Created by Nikita Georgiou on 5/9/16.
//  Copyright © 2016 Nikita Georgiou. All rights reserved.
//

#include "replicator.hpp"

namespace backend{
    replicator::replicator(const char* port, std::vector<std::pair<std::string, bool> > ports):ports(ports){}
    
    /*Ping every alive server, if their timestamp older then send them our data
     *if a server doesn't respond then ignore it and keep going*/
    unsigned replicator::multicast(){
        for(std::pair<std::string, bool> p : ports){
            if(p.second){
                
            }
        }
        return 1;
    }
    
    bool replicator::compare(unsigned otherStamp){
        return timestamp > otherStamp;
    }
    
    //bool replicator::ping(std::string port);
    bool pong();
    unsigned update(unsigned newStamp);
    void heartbeat();

}