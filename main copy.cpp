#include <iostream> 
#include <sys/socket.h>
#include <vector>
#include <map>
#include <utility>
#include <netdb.h>
#include <cstring>
#include <sstream> 
#include <unistd.h>
#include <mutex>
#include <thread>
#include <chrono>

#include "connection.h"
#include "surveyData.h"
#include "user.h"
#include "sessionMap.h"
#include "userMap.h"
#include "surveyMap.h"

#define NUM_THREADS thread::hardware_concurrency()

using namespace std;
using namespace backend;

//Validate the user's username to make sure it's legal,
//ie contains no special characters and is between 3 and 16 characters long
bool validateRegistration(const string& uname, const string& pass){
    if(uname.size() > 16 || uname.size() < 3 || pass.size() < 1)
        return false;
    
    for(char c : uname)
        if(!((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122)))
            return false;
    
    return true;
}

//Make sure survey parameters are legal
//ie contains only alphanumeric + punctuation
bool validateSurveyFields(const string& data){
    //split the arguments up to be analyzed indipendantly
    stringstream s;
    s.str(data);
    char tmp[1000];
    vector<string> args(4);
    int i = 0;
    while(s.getline(tmp, data.size(), '\t')){
        args[i++] = tmp;
    }
    
    //args 1: question; 2: option1; 3: option2
    if(args[1].size() < 1 || args[1].size() > 100 || args[2].size() < 2 || args[2].size() > 100 || args[3].size() < 2 || args[3].size() > 100)
        return false;
    
    for(string s : args)
        for(char c: s)
            if(c < 32 || c > 126)
                return false;

    return true;
}

//take in the username of the survey's creator, a string of survey details
//delimited by \t and a pointer to the list of currently existing surveys
//return success '1' char if survey was added
//DEPRICATED returning the pointer to the created survey as this operation
//should be pseudo-atomic!
char createSurvey(const string& uname, const string& data, surveyMap* listOfSurveys){
	stringstream s;
	s.str(data);
	char tmp[1000];
	vector<string> args(4);
	int i = 0;
	while(s.getline(tmp, data.size(), '\t')){
		args[i++] = tmp;
	}
	listOfSurveys->addSurvey(args[0], new surveyData(uname, args[0], args[1], args[2], args[3]));
    return '1';
}

//Decide what to do based on first vector item. All following items are supplimentary
//new_sd and data are used to talk back to python. Lists are to acces/modify shared resources.
//Indices are specified as 1: argument at vector index 1, etc
void logic(string incoming_data_buffer, const ssize_t bytes_recieved, int new_sd, surveyMap* listOfSurveys, userMap* listOfUsers, sessionMap* session){
    //break up argument string into individual arguments for later operations
    //args are delimited by \n, which is default delimited for getline. Loop through the args storing each one in vector
    char data[1024];
    int i = 0;
    stringstream incoming_data_stream;
    vector<string> args(6);

    incoming_data_stream.str(incoming_data_buffer);
    cout << "incoming data buffer: " << incoming_data_buffer << endl;
    while(incoming_data_stream.getline(data, bytes_recieved)){
        // 	cout << "while loop data: " << data << endl;
        args[i++] = data;
    }
    //debugging...
    cout << "Bytes recieved: " << bytes_recieved << endl;
    incoming_data_buffer[bytes_recieved] = '\0';
    //cout << "stirng stream: " << incoming_data_stream << endl;

	if(args[0] == "quit");
	//validates whether the username and password submitted exist and match
    //1: username; 2: password
	else if(args[0] == "validLogin"){
		data[0] = listOfUsers->validLogin(args[1], args[2]) ? '1' : '0';
		send(new_sd, data, 1, 0);
	}
	//marks user as having an active session
    //1: username
	else if(args[0] == "logMeIn"){
        cout << "executing logmein" << endl;
		session->joinSession(args[1]);
        send(new_sd, data, 0, 0);
        cout << "executed logmein" << endl;
	}
	//checks if user is already in the session
    //1: username
	else if(args[0] == "inSession"){
		data[0] = session->inSession(args[1]) ? '1' : '0';
		send(new_sd, data, 1, 0);
	}
	//check if that username is already registered
    //1: username
	else if(args[0] == "isRegistered"){
		data[0] = listOfUsers->isRegistered(args[1]) ? '1' : '0';
		send(new_sd, data, 1, 0);
	}
	//create a new user, returns '1' if username is successfully created ("valid")
    //and 0 if otherwise
    //1: username; 2: password
	else if(args[0] == "register"){
        if(validateRegistration(args[1], args[2]))
            data[0] = listOfUsers->addUser(args[1], args[2]);
        else
            data[0] = '0';
        send(new_sd, data, 1, 0);
	}
	//removes a user from the session map
    //1: username
	else if(args[0] == "logout"){
		session->leaveSession(args[1]);
	}
	//removes user from list of users and session
    //1: username
	else if(args[0] == "deleteAccount"){
		listOfUsers->removeUser(args[1]);
	}
    //returns concatinated string of all existing survey names
	else if(args[0] == "surveyNames"){
    //    cout << "getting survey names: " << listOfSurveys->getSurveyNames() << endl;
		send(new_sd, listOfSurveys->getSurveyNames().c_str(), listOfSurveys->getSurveyNames().size(), 0);
	}
    //sends the string description of a particular survey with expected \n delimiter
    //1: survey name
	else if(args[0] == "getSurvey"){
		surveyData* surveyName = listOfSurveys->getSurvey(args[1]);
		cout << "tostirng data: " << surveyName->toString() << endl;
		if(surveyName)
			send(new_sd, surveyName->toString().c_str(), surveyName->toString().size(), 0);
	}
    //creates a new survey
    //1: survey creator's username; 2: survey details
	else if(args[0] == "createSurvey"){
    //    cout << "creating survey with: " + args[1] + " and " + args[2] << endl;
        if(validateSurveyFields(args[2]))
            data[0] = createSurvey(args[1], args[2], listOfSurveys);
        else
            data[0] = '0';
        send(new_sd, data, 1, 0);
	}
    //Votes for an option in a survey. Doesn't do anything if user has
    //already voted on that survey or if they're anonymous
    //1: option number; 2: voter's username; 3: survey's name
	else if(args[0] == "vote"){
        string vote;
        data[0] = listOfSurveys->vote(args[1], args[2], args[3]);
        if(args[1] == "1")
            vote = listOfSurveys->getSurvey(args[3])->getOption1();
        else
            vote = listOfSurveys->getSurvey(args[3])->getOption2();
        if(data[0] == '1')
            listOfUsers->getUser(args[2])->vote(vote, args[3]);
        send(new_sd, data, 1, 0);
	}
    //returns a concatinated string of pre-formatted messages reporting who
    //has voted for what on a particular survey, delimited by \n.
    //1: survey's nam
	else if(args[0] == "whoVoted"){
		string surveyName = listOfSurveys->whoVoted(args[1]);
		if(surveyName != "0"){
			send(new_sd, surveyName.c_str(), surveyName.size(), 0);
		}
	}
    //returns concatinated string of pre-formatted messages reporting on what
    //and how a particular user has voted, delimited by \n.
    //1: username
	else if(args[0] == "votedOn"){
		string surveyNames = listOfUsers->votedOn(args[1]);
		if(surveyNames != "0")
			send(new_sd, surveyNames.c_str(), surveyNames.size(), 0);
    }
    close(new_sd);
    cout << "exiting logic" << endl;
    return;
}

void socketMess(){
	//communication vars
	vector<string> args(4); //To store the function to call, with supplimentary agruments
	stringstream incoming_data_stream; //so we can parse one large string into arguments mentioend above
	char data[1000]; //to store responses to python
	int i = 0;
	ssize_t bytes_recieved;
	char incoming_data_buffer[1000]; //raw bytes recieved from python
	int new_sd;
	struct sockaddr_storage their_addr;
	string surveyNames = "";
	socklen_t addr_size;
    string container;
    
    //thread array
    vector<thread> threads(NUM_THREADS);
    unsigned threadCounter = 0;
    for(thread& t : threads){
        t = thread();
        cout << "instantiating thread " << t.get_id() << endl;
    }

	sessionMap* session = new sessionMap();
	userMap* listOfUsers = new userMap();
	surveyMap* listOfSurveys = new surveyMap();

    connection conn;
    int status = conn.getStatus();
    int socketfd = conn.getSocketfd();
    cout << "Listening for connection..." << endl;
    status = listen(conn.getSocketfd(), 5);
    if (status == -1)  cout << "listen error" << endl;
    
    while(true){
		addr_size = sizeof(their_addr);
        cout << "about to accept" << endl;
		new_sd = accept(conn.getSocketfd(), (struct sockaddr *)&their_addr, &addr_size);
		if (new_sd == -1)
		{
		    cout << "listen error" << endl;
		}
		else
		{
		    cout << "Connection accepted. Using new socketfd : "  <<  new_sd << endl;
		}

		cout << "Waiting to recieve data" << endl;
		bytes_recieved = recv(new_sd, incoming_data_buffer, 1000, 0);
		if (bytes_recieved == 0){
			cout << "host shut down." << endl;
		}
	    if (bytes_recieved == -1) cout << "recieve error!" << endl; //debugging
	    
        if(bytes_recieved > 0){
            container = incoming_data_buffer;
            if(threads[(threadCounter + 1) % NUM_THREADS].joinable()){
                threads[(threadCounter + 1) % NUM_THREADS].join();
            }
            threads[threadCounter % NUM_THREADS] = thread(logic, container, bytes_recieved, new_sd, listOfSurveys, listOfUsers, session);
            cout << "assigned thread " << MOD_NUM_THREADS << " with unique id " << threads[MOD_NUM_THREADS].get_id() << endl;
        }
        ++threadCounter;

        //cleaning buffers
        for(string& str : args)
			str = "";
		memset(data, 0, sizeof(data));
		i = 0;
		memset(incoming_data_buffer, 0, sizeof(incoming_data_buffer));
		bytes_recieved = 0;
		incoming_data_stream.str("");
		incoming_data_stream.clear();
		surveyNames = "";
        container = "";
		}

	}

int main(){
	socketMess();
}

















