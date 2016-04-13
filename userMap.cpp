#include <string>
#include <mutex>
#include <map>

#include "userMap.h"
#include "user.h"

namespace backend{
	userMap::userMap(){}

	void userMap::addUser(const std::string& uname, const std::string& pword){
		lock.lock();
		listOfUsers[uname] = new user(uname, pword);
		lock.unlock();
	}

	bool userMap::removeUser(const std::string& uname){
		lock.lock();
		try{
			delete listOfUsers.at(uname);
			listOfUsers.erase(uname);
			lock.unlock();
			return true;
		}
		catch(std::out_of_range& ex){
			lock.unlock();
			return false;
		}
	}

	void userMap::clearUsers(){
	lock.lock();
	for(std::map<std::string, user*>::iterator i = listOfUsers.begin(); i != listOfUsers.end(); ++i){
		delete i->second;
	}
	listOfUsers.clear();
	lock.unlock();
	}

	std::string userMap::votedOn(const std::string& uname)const{
		std::lock_guard<std::mutex> lockg(lock);
		try{
			return listOfUsers.at(uname)->votedOn();
		}
		catch(std::out_of_range& ex){
			return "0";
		}
		return "0";
	}

	bool userMap::isRegistered(const std::string& uname)const{
		std::lock_guard<std::mutex> lockg(lock);
		return listOfUsers.count(uname) > 0;
	}

	user* userMap::getUser(const std::string& uname)const{
		std::lock_guard<std::mutex> lockg(lock);
		try{
			return listOfUsers.at(uname);
		}
		catch(std::out_of_range& ex){
			return NULL;
		}
		return NULL;
	}

	bool userMap::validLogin(const std::string& uname, const std::string& pword)const{
		std::lock_guard<std::mutex> lockg(lock);
		try{
			return listOfUsers.at(uname)->getPass() == pword;
		}
		catch(std::out_of_range& ex){
			return false;
		}
		return false;
	}
}