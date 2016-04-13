#include <string> 
#include <mutex>
#include <map>
#include <iostream>

#include "surveyData.h"
#include "surveyMap.h"


namespace backend{
	surveyMap::surveyMap(){}

	void surveyMap::addSurvey(const std::string& name, surveyData* survey){
		lock.lock();
		listOfSurveys[name] = survey;
		lock.unlock();
	}

	char surveyMap::vote(const std::string& option, const std::string& uname, const std::string& survey){
		lock.lock();
		if(uname != "Anon" && !listOfSurveys[survey]->hasVoted(uname)){
			if(option == "1")
				listOfSurveys[survey]->option1Vote(uname);
			else if(option == "2")
				listOfSurveys[survey]->option2Vote(uname);
			lock.unlock();
			return '1';
		}
		lock.unlock();
		return '0';
	}

	bool surveyMap::removeSurvey(const std::string& name){
		lock.lock();
		try{
			delete listOfSurveys.at(name);
			listOfSurveys.erase(name);
			lock.unlock();
			return true;
		}
		catch(std::out_of_range& ex){
			lock.unlock();
			return false;
		}
	}

	std::string surveyMap::getSurveyNames(){
		std::string surveyNames = "";
		lock.lock();
		for(std::map<std::string, surveyData*>::iterator i = listOfSurveys.begin(); i != listOfSurveys.end(); ++i)
			surveyNames += i->first + '\t';
		lock.unlock();
		return surveyNames;
	}

	void surveyMap::clearSurveys(){
		lock.lock();
		for(std::map<std::string, surveyData*>::iterator i = listOfSurveys.begin(); i != listOfSurveys.end(); ++i){
			delete i->second;
		}
		listOfSurveys.clear();
		lock.unlock();
	}

	surveyData* surveyMap::getSurvey(const std::string& name)const{
		std::lock_guard<std::mutex> lockg(lock);
		try{
			return listOfSurveys.at(name);
		}
		catch(std::out_of_range& ex){
			return NULL;
		}
	}

	std::string surveyMap::whoVoted(const std::string& name)const{
		std::lock_guard<std::mutex> lockg(lock);
		try{
			return listOfSurveys.at(name)->votedFor();
		}
		catch(std::out_of_range& ex){
			return "0";
		}
		return "0";
	}
}