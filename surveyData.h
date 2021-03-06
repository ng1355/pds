#ifndef SURVEYDATA_H_1234
#define SURVEYDATA_H_1234 1

#include <string>
#include <map>
#include <utility>
#include <mutex>

#include "fileManager.hpp"

namespace backend{
class surveyData{
public:
	surveyData(std::string owner, std::string name, std::string question, std::string option1, std::string option2);
    
    friend void fileManager::populateSurveyMap();

	std::pair<int, int> percentage()const;
	void option1Vote(const std::string& uname);
	void option2Vote(const std::string& uname);
	//getters
	std::string getOwner()const;
	std::string getName()const;
	std::string getQuestion()const;
	std::string getOption1()const;
	std::string getOption2()const;
	int getnumVoted()const;
	float getOption1Vote()const;
	float getOption2Vote()const;
	bool hasVoted(const std::string& uname)const;
    
    //setters
    void setNumVoted(unsigned newNumVoted);
    void setOption1Votes(float newVote);
    void setOption2Votes(float newVote);

	std::string votedFor();
	std::string toString()const;
    std::string save();
private:
	std::string owner, name, question, option1, option2;
	unsigned numVoted;
	float option1Votes, option2Votes;
	std::map<std::string, std::string> usersVoted;
    mutable std::mutex lock;
};
}
#endif