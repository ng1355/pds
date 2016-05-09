#include <string>
#include <map>
#include <utility>
#include <mutex>

#include "user.h"
#include "surveyData.h" 
#include "surveyMap.h"

namespace backend{
	surveyData::surveyData(std::string owner, std::string name, std::string question, std::string option1, std::string option2): owner(owner), name(name), question(question), option1(option1), option2(option2), option1Votes(0), option2Votes(0), numVoted(0){}

	std::pair<int, int> surveyData::percentage()const {
        std::lock_guard<std::mutex> lockg(lock);
        if(option1Votes + option2Votes == 0)
			return std::make_pair(0, 0);
		return std::make_pair((option1Votes / (option1Votes + option2Votes) * 100), (option2Votes / (option1Votes + option2Votes) * 100));
	}

	void surveyData::option1Vote(const std::string& uname){
        std::lock_guard<std::mutex> lockg(lock);
		++option1Votes;
		++numVoted;
		usersVoted[uname] = option1;
	}

	void surveyData::option2Vote(const std::string& uname){
        std::lock_guard<std::mutex> lockg(lock);
		++option2Votes;
		++numVoted;
		usersVoted[uname] = option2;
	}

	//getters
    //these can't be modified after construction
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
    //these can be
	int surveyData::getnumVoted()const {
        std::lock_guard<std::mutex> lockg(lock);
		return numVoted;
	}
	float surveyData::getOption1Vote()const {
        std::lock_guard<std::mutex> lockg(lock);
		return option1Votes;
	}
	float surveyData::getOption2Vote()const {
        std::lock_guard<std::mutex> lockg(lock);
		return option2Votes;
	}

	bool surveyData::hasVoted(const std::string& uname)const {
        return usersVoted.count(uname) > 0;
	}

	std::string surveyData::votedFor(){
		std::string names = "";
        std::lock_guard<std::mutex> lockg(lock);
		for(std::map<std::string, std::string>::iterator i = usersVoted.begin(); i != usersVoted.end(); i++)
			names += i->first + " voted for " + i->second + "!\n"; //move this to fronend so usernames can be links to profile
		return names;
	}
    
    void surveyData::setNumVoted(unsigned newNumVoted){
        numVoted = newNumVoted;
    }
    void surveyData::setOption1Votes(float newVote){
        option1Votes = newVote;
    }
    void surveyData::setOption2Votes(float newVote){
        option2Votes = newVote;
    }

	std::string surveyData::toString()const {
        //std::lock_guard<std::mutex> lockg(lock);
        //for some reason locking this function causes logicPath2 in main.cpp to quit early resulting
        //in an indefinite SIGSTOP hang on accept().
        //will have to find another way to handle this, hopefully without having to lock entire map.
		return name + '\n' + question + '\n' + option1 + '\n' + option2 + '\n' + owner + '\n' + std::to_string(std::get<0>(percentage())) + '\n' + std::to_string(std::get<1>(percentage())) + '\n' + std::to_string(numVoted);
	}
    
    std::string surveyData::save(){
        std::string output;
        std::lock_guard<std::mutex> lockg(lock);
        output = owner + '\0' + name + '\0' + question + '\0' + option1 + '\0' + option2 + '\0' + std::to_string(numVoted) + '\0' + std::to_string((int)option1Votes) + '\0' + std::to_string((int)option2Votes)+ '\0';
        for(std::map<std::string, std::string>::iterator i = usersVoted.begin(); i != usersVoted.end(); ++i)
            output += i->first + '\0' + i->second + '\0';
        return output;
    }
}



