#include <string> 
#include <mutex>
#include <map>
#include <iostream>

#include "surveyData.h"
#include "surveyMap.h"


namespace backend{
	surveyMap::surveyMap(){}

	void surveyMap::addSurvey(const std::string& name, surveyData* survey){
        std::lock_guard<std::mutex> lockg(lock);
		listOfSurveys[name] = survey;
	}

	char surveyMap::vote(const std::string& option, const std::string& uname, const std::string& survey){
		if(uname != "Anon" && !listOfSurveys[survey]->hasVoted(uname)){
			if(option == "1")
				listOfSurveys[survey]->option1Vote(uname);
			else if(option == "2")
				listOfSurveys[survey]->option2Vote(uname);
			return '1';
		}
		return '0';
	}

	bool surveyMap::removeSurvey(const std::string& name){
        std::lock_guard<std::mutex> lockg(lock);
        try{
			delete listOfSurveys.at(name);
			listOfSurveys.erase(name);
			return true;
		}
		catch(std::out_of_range& ex){
			return false;
		}
	}

	std::string surveyMap::getSurveyNames(){
		std::string surveyNames = "";
        std::lock_guard<std::mutex> lockg(lock);
        for(std::map<std::string, surveyData*>::iterator i = listOfSurveys.begin(); i != listOfSurveys.end(); ++i)
			surveyNames += i->first + '\t';
		return surveyNames;
	}

	void surveyMap::clearSurveys(){
        std::lock_guard<std::mutex> lockg(lock);
        for(std::map<std::string, surveyData*>::iterator i = listOfSurveys.begin(); i != listOfSurveys.end(); ++i){
			delete i->second;
		}
		listOfSurveys.clear();
	}

	surveyData* surveyMap::getSurvey(const std::string& name)const{
		try{
			return listOfSurveys.at(name);
		}
		catch(std::out_of_range& ex){
			return NULL;
		}
	}

	std::string surveyMap::whoVoted(const std::string& name)const{
		try{
			return listOfSurveys.at(name)->votedFor();
		}
		catch(std::out_of_range& ex){
			return "0";
		}
		return "0";
	}
    
    std::map<std::string, surveyData*>::iterator surveyMap::begin(){
        return listOfSurveys.begin();
    }
    
    std::map<std::string, surveyData*>::iterator surveyMap::end(){
        return listOfSurveys.end();
    }
}