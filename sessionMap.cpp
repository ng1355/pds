#include "sessionMap.h"

#include <string>
#include <mutex>
#include <map>

namespace backend{
	void sessionMap::joinSession(const std::string& uname){
		lock.lock();
		session[uname] = uname;
		lock.unlock();
	}

	bool sessionMap::inSession(const std::string& uname)const{
		std::lock_guard<std::mutex> lockg(lock);
		return session.count(uname) > 0;
	}

	bool sessionMap::leaveSession(const std::string& uname){
		lock.lock();
		try{
			session.at(uname);
			session.erase(uname);
			lock.unlock();
			return true;
		}
		catch(std::out_of_range& ex){
			lock.unlock();
			return false;
		}
	}
	
	void sessionMap::clearSession(){
		lock.lock();
		session.clear();
		lock.unlock();
	}
}