#ifndef USERMAP_H_1234
#define USERMAP_H_1234 1

#include <string> 
#include <mutex>
#include <map>

namespace backend{
class user;
class userMap{
public:
	userMap();
	void addUser(const std::string& uname, const std::string& pword);
	bool removeUser(const std::string& uname);
	void clearUsers();
	user* getUser(const std::string& uname)const;
	bool isRegistered(const std::string& uname)const;
	std::string votedOn(const std::string& uname)const;
	bool validLogin(const std::string& uname, const std::string& pword)const;
private:
	std::map<std::string, user*> listOfUsers;
	mutable std::mutex lock;
};
}
#endif