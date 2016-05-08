#ifndef USER_H_1234
#define USER_H_1234 1

#include <string>
#include <vector>
#include <utility>
#include <mutex>

#include "fileManager.hpp"

namespace backend{
	class user{
	public:
		user(const std::string& uname, const std::string& pword);
		std::string votedOn();
		void vote(const std::string& choice, const std::string& survey);
		std::string getName()const;
		std::string getPass()const;
        friend void fileManager::populateUserMap();
        std::string save()const;
	private:
        std::string uname;
        std::string pword;
		std::vector<std::pair<std::string, std::string> > votes;
        mutable std::mutex lock;
	};
}
#endif