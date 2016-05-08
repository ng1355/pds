//
//  fileManager.hpp
//  backend
//
//  Created by Nikita Georgiou on 4/29/16.
//  Copyright © 2016 Nikita Georgiou. All rights reserved.
//

#ifndef fileManager_hpp
#define fileManager_hpp

#include <stdio.h>
#include <mutex>
#include <fstream>
#include <map> 
#include <string>

#include "userMap.h"
#include "surveyMap.h"

namespace backend{
    class surveyMap;
    class fileManager{
    public:
        fileManager(userMap* users, surveyMap* surveys, const std::string usrDir, const std::string);
        void commit();
        void load();
        void populateUserMap();
        void populateSurveyMap();
        void saveUserMap();
        void saveSurveyMap();
        void clearUserMap();
        void clearSurveyMap();
        void newUserMap(userMap* users);
        void newSurveyMap(surveyMap* surveys);
        void setUsrDir(const std::string newUsrDir);
        void setSurvDir(const std::string newSurvDir);
    private:
        mutable std::mutex lock;
        userMap* users;
        surveyMap* surveys;
        std::string usrDir, survDir;
    };
    
}
#endif /* fileManager_hpp */
