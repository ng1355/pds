#ifndef SURVEYMAP_H_1234
#define SURVEYMAP_H_1234 1

#include <string> 
#include <mutex>
#include <map>

#include "fileManager.hpp"

namespace backend{
class surveyData;
class userMap;
class surveyMap{
public:
	surveyMap();
	void addSurvey(const std::string& name, surveyData* survey);
	char vote(const std::string& option, const std::string& uname, const std::string& survey);
	bool removeSurvey(const std::string& name);
	std::string getSurveyNames();
	void clearSurveys();
	surveyData* getSurvey(const std::string& name)const;
	std::string whoVoted(const std::string& name)const;
    std::map<std::string, surveyData*>::iterator begin();
    std::map<std::string, surveyData*>::iterator end();
private:
	std::map<std::string, surveyData*> listOfSurveys;
	mutable std::mutex lock;
};
}
#endif