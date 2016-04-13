#ifndef SESSIONMAP_H_1234
#define SESSIONMAP_H_1234 1

#include <string>
#include <map>
#include <mutex>

namespace backend{
class sessionMap{
public:
	void joinSession(const std::string& uname);
	bool inSession(const std::string& uname)const;
	bool leaveSession(const std::string& uname);
	std::string votedOn(const std::string& uname);
	void clearSession();
private:
	std::map<std::string, std::string> session;
	mutable std::mutex lock;
};
}
#endif