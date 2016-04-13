#include <string>
#include <map>
#include <utility>

#include "user.h"
#include "surveyData.h" 
#include "surveyMap.h"

namespace backend{
	surveyData::surveyData(std::string owner, std::string name, std::string question, std::string option1, std::string option2): owner(owner), name(name), question(question), option1(option1), option2(option2), option1Votes(0), option2Votes(0), numVoted(0){}

	std::pair<int, int> surveyData::percentage()const {
		if(option1Votes + option2Votes == 0)
			return std::make_pair(0, 0);
		return std::make_pair((option1Votes / (option1Votes + option2Votes) * 100), (option2Votes / (option1Votes + option2Votes) * 100));
	}

	void surveyData::option1Vote(const std::string& uname){
		++option1Votes;
		++numVoted;
		usersVoted[uname] = option1;
	}

	void surveyData::option2Vote(const std::string& uname){
		++option2Votes;
		++numVoted;
		usersVoted[uname] = option2;
	}

	//getters
	std::string surveyData::getOwner()const {
		return owner;
	}
	std::string surveyData::getName()const {
		return name;
	}
	std::string surveyData::getQuestion()const {
		return question;
	}
	std::string surveyData::getOption1()const {
		return option1;
	}
	std::string surveyData::getOption2()const {
		return option2;
	}
	int surveyData::getnumVoted()const {
		return numVoted;
	}
	float surveyData::getOption1Vote()const {
		return option1Votes;
	}
	float surveyData::getOption2Vote()const {
		return option2Votes;
	}

	bool surveyData::hasVoted(const std::string& uname)const {
	return usersVoted.count(uname) > 0;
	}

	std::string surveyData::votedFor(){
		std::string names = "";
		for(std::map<std::string, std::string>::iterator i = usersVoted.begin(); i != usersVoted.end(); i++)
			names += i->first + " voted for " + i->second + "!\n"; //move this to fronend so usernames can be links to profile
		return names;
	}

	std::string surveyData::toString()const {
		return name + '\n' + option1 + '\n' + option2 + '\n' + owner + '\n' + std::to_string(std::get<0>(percentage())) + '\n' + std::to_string(std::get<1>(percentage())) + '\n' + std::to_string(numVoted);
	}
}



