#include <string>
#include <mutex>
#include <map>

#include "userMap.h"
#include "user.h"

namespace backend{
	userMap::userMap(){}

	user* userMap::addUser(const std::string& uname, const std::string& pword){
        std::lock_guard<std::mutex> lockg(lock);
		users[uname] = new user(uname, pword);
        return users[uname];
	}

	bool userMap::removeUser(const std::string& uname){
        std::lock_guard<std::mutex> lockg(lock);
		try{
			delete users.at(uname);
			users.erase(uname);
			return true;
		}
		catch(std::out_of_range& ex){
			return false;
		}
	}

	void userMap::clearUsers(){
    std::lock_guard<std::mutex> lockg(lock);
	for(std::map<std::string, user*>::iterator i = users.begin(); i != users.end(); ++i){
		delete i->second;
	}
	users.clear();
	}

	std::string userMap::votedOn(const std::string& uname)const{
		try{
			return users.at(uname)->votedOn();
		}
		catch(std::out_of_range& ex){
			return "0";
		}
		return "0";
	}

	bool userMap::isRegistered(const std::string& uname)const{
		return users.count(uname) > 0;
	}

	user* userMap::getUser(const std::string& uname)const{
		try{
			return users.at(uname);
		}
		catch(std::out_of_range& ex){
			return NULL;
		}
		return NULL;
	}

	bool userMap::validLogin(const std::string& uname, const std::string& pword)const{
		try{
			return users.at(uname)->getPass() == pword;
		}
		catch(std::out_of_range& ex){
			return false;
		}
		return false;
	}

    std::map<std::string, user*>::iterator userMap::begin(){
        return users.begin();
    }
    std::map<std::string, user*>::iterator userMap::end(){
        return users.end();
    }
    
    
}