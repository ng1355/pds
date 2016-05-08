#ifndef USERMAP_H_1234
#define USERMAP_H_1234 1

#include <string> 
#include <mutex>
#include <map>

#include "fileManager.hpp"

namespace backend{
class user;
class userMap{
public:
	userMap();
	user* addUser(const std::string& uname, const std::string& pword);
	bool removeUser(const std::string& uname);
	void clearUsers();
	user* getUser(const std::string& uname)const;
	bool isRegistered(const std::string& uname)const;
	std::string votedOn(const std::string& uname)const;
	bool validLogin(const std::string& uname, const std::string& pword)const;
    std::map<std::string, user*>::iterator begin();
    std::map<std::string, user*>::iterator end();
private:
	std::map<std::string, user*> users;
	mutable std::mutex lock;
};
}
#endif