#include <utility>
#include <string>
#include <vector>

#include "user.h"

namespace backend{
	//user.h
	user::user(const std::string& uname, const std::string& pword): uname(uname), pword(pword){}

	std::string user::votedOn(){
		std::string str = "";
		for(std::pair<std::string, std::string> p : votes)
			str += "Voted " + std::get<0>(p) + " on survey " + std::get<1>(p) + "!\n"; //move to frontend for survey links
		return str;
	}

	void user::vote(const std::string& choice, const std::string& survey){
        votes.push_back(std::make_pair(choice, survey));
	}

	std::string user::getName()const {
		return uname;
	}
	std::string user::getPass()const {
		return pword;
	}

	//
}