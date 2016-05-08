#include "sessionMap.h"

#include <string>
#include <mutex>
#include <map>

namespace backend{
	void sessionMap::joinSession(const std::string& uname){
        std::lock_guard<std::mutex> lockg(lock);
        session[uname] = uname;
	}

	bool sessionMap::inSession(const std::string& uname)const{
		return session.count(uname) > 0;
	}

	bool sessionMap::leaveSession(const std::string& uname){
        std::lock_guard<std::mutex> lockg(lock);
        try{
			session.at(uname);
			session.erase(uname);
			return true;
		}
		catch(std::out_of_range& ex){
			return false;
		}
	}
	
	void sessionMap::clearSession(){
        std::lock_guard<std::mutex> lockg(lock);
		session.clear();
	}
}