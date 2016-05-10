//
//  fileManager.cpp
//  backend
//
//  Created by Nikita Georgiou on 4/29/16.
//  Copyright © 2016 Nikita Georgiou. All rights reserved.
//

#include <vector>
#include <iostream>
#include <fstream> 
#include <map>

#include "user.h"
#include "surveyData.h"
#include "fileManager.hpp"
#include "userMap.h"
#include "surveyMap.h"

#define trunc std::ios::in | std::ios::trunc

namespace backend{
    fileManager::fileManager(userMap* users,  surveyMap* surveys, std::string usrDir, std::string survDir): usrDir(usrDir), survDir(survDir), users(users), surveys(surveys){}
    
    void fileManager::commit(){
        saveUserMap();
        saveSurveyMap();
    }
    
    void fileManager::load(){
        populateUserMap();
        populateSurveyMap();
    }
    
    //Reads data from a file with saved user data, and parses it into the lsitOfUsers map for memory access
    //gets an individual line from the file, then parses tokens from that line delimited by \0 to be stored in a vector
    //then, adds the user to the map with the username as the first item, password as second.
    //All subsequent items are the surveys and choices made by the user, which are cast into pairs
    //as such, the loop iterates over every two items, before directly pushing back the created pair into the vector
    //as this method is a friend of user's.
    void fileManager::populateUserMap(){
        std::lock_guard<std::mutex> lockg(lock);
        std::ifstream input(usrDir);
        std::string line = "";
        user* tmp;
        std::vector<std::string> args;
        size_t start, end;
        while(getline(input, line)){
            start = 0; end = 0;
            while((end = line.find('\0', start)) != std::string::npos){
                args.push_back(line.substr(start, end - start));
                start = end + 1;
            }
            tmp = users->addUser(args[0], args[1]);
            for(size_t index = 2; index <= args.size() - 2; index += 2)
                tmp->votes.push_back(std::make_pair(args[index], args[index + 1]));
            args.clear();
        }
        input.close();
    }

        void fileManager::populateSurveyMap(){
            std::lock_guard<std::mutex> lockg(lock);
            std::ifstream input(survDir, std::ios::in);
            std::string line = "";
            surveyData* tmp;
            std::vector<std::string> args;
            size_t start, end;
            while(getline(input, line)){
                start = 0, end = 0;
                while((end = line.find('\0', start)) != std::string::npos){
                    args.push_back(line.substr(start, end - start));
                    start = end + 1;
                }
                surveys->addSurvey(args[1], tmp = new surveyData(args[0], args[1], args[2], args[3], args[4]));
                tmp->setNumVoted(std::stoi(args[5]));
                tmp->setOption1Votes(std::stof(args[6]));
                tmp->setOption2Votes(std::stof(args[7]));
                std::cout << "size is: " << args.size() << std::endl;
                for(size_t index = 8; index <= args.size() - 2; index += 2)
                    tmp->usersVoted[args[index]] = args[index + 1];
                args.clear();
            }
            input.close();
        }
    
    void fileManager::saveUserMap(){
        std::lock_guard<std::mutex> lockg(lock);
        std::ofstream output(usrDir, trunc);
        for(std::map<std::string, user*>::iterator i = users->begin(); i != users->end(); ++i){
            output.write(i->second->save().c_str(), i->second->save().size());
            output << '\n';
        }
        output.close();
    }
    
    void fileManager::saveSurveyMap(){
        std::lock_guard<std::mutex> lockg(lock);
        std::ofstream output(survDir, trunc);
        for(std::map<std::string, surveyData*>::iterator i = surveys->begin(); i != surveys->end(); ++i){
            output.write(i->second->save().c_str(), i->second->save().size());
            output << '\n';
        }
        output.close();
    }
    
    void fileManager::clearUserMap(){
        users->clearUsers();
    }
    
    void fileManager::clearSurveyMap(){
        surveys->clearSurveys();
    }
    
    void fileManager::newUserMap(userMap* newUsers){
        clearUserMap();
        users = newUsers;
    }
    
    void fileManager::newSurveyMap(surveyMap* newSurveys){
        clearSurveyMap();
        surveys = newSurveys;
    }
    
    void fileManager::setUsrDir(const std::string newUsrDir){
        usrDir = newUsrDir;
    }
    
    void fileManager::setSurvDir(const std::string newSurvDir){
        survDir = newSurvDir;
    }

}