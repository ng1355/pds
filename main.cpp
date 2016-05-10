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
#include "userMap.h"
#include "sessionMap.h"
#include "surveyMap.h"
#include "fileManager.hpp"

using namespace std;
using namespace backend;

//Allows for control from command line
void CLI(){
    int new_sd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    
    char incoming_data_buffer[1024];
    char data[1024];
    stringstream s;
    ssize_t bytes_recieved = 0;
    string arg = "";
    
    connection conn;
    conn.server(nullptr, "1337");
    int status = conn.getStatus();
    cout << "CLI active..." << endl;
    status = listen(conn.getSocketfd(), 5);
    if (status == -1)  cout << "listen error" << endl;
    
    while(true){
        addr_size = sizeof(their_addr);
        new_sd = accept(conn.getSocketfd(), (struct sockaddr *)&their_addr, &addr_size);
        if (new_sd == -1)
        {
            cout << "listen error" << endl;
        }
        
        cout << "Waiting to recieve data" << endl;
        bytes_recieved = recv(new_sd, incoming_data_buffer, 1024, 0);
        if (bytes_recieved == 0) cout << "host shut down." << endl;
        else if (bytes_recieved == -1) cout << "recieve error!" << endl;
        
        //debugging...
        cout << "Bytes recieved: " << bytes_recieved << endl;
        incoming_data_buffer[bytes_recieved] = '\0';
        cout << "data buff: " << incoming_data_buffer << endl;
        
        
        arg =
        close(new_sd);
    }

}

//continuously saves users and surveys in memory to a file
//every interval of time specified by the second param.
void saver(fileManager* fm, unsigned time = 15){
    while(true){
        this_thread::sleep_for(chrono::seconds(time));
        cout << "Saving...\n";
        fm->commit();
    }
}

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

//listens on a socket descriptor and then parses the message into an array of
//arguments. Assumes a newline (\n) delimiter.
vector<string> reciever(int new_sd){
    //vars
    char incoming_data_buffer[1024];
    char data[1024];
    stringstream s;
    ssize_t bytes_recieved = 0;
    int i = 0;
    vector<string> args(6);
    
    cout << "Waiting to recieve data" << endl;
    bytes_recieved = recv(new_sd, incoming_data_buffer, 1024, 0);
    if (bytes_recieved == 0) cout << "host shut down." << endl;
    else if (bytes_recieved == -1) cout << "recieve error!" << endl;
    
    //debugging...
    cout << "Bytes recieved: " << bytes_recieved << endl;
    incoming_data_buffer[bytes_recieved] = '\0';
    cout << "data buff: " << incoming_data_buffer << endl;
    
    s.str(incoming_data_buffer);
    
    //break up argument string into individual arguments for later operations
    //args are delimited by \n, which is default delimited for getline. Loop through the args storing each one in vector
    while(s.getline(data, bytes_recieved))
        args[i++] = data;
    
    return args;
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

//Reciev data through the passed socket file descriptor, then parse the data into a vector of string.
//Decide what to do based on first vector item. All following items are supplimentary
//new_sd and data are used to talk back to python. Lists are to access/modify shared resources.
//Indices are specified as 1: argument at vector index 1, etc
void logicPath1(int new_sd, vector<string>& args, surveyMap* listOfSurveys, userMap* listOfUsers, sessionMap* session){
    char data[1];
    args[1];
    args[2];
	if(args[0] == "quit"); //Still working on how to impliment this.
	//validates whether the username and password submitted exist and match
    //1: username; 2: password
	else if(args[0] == "validLogin"){
		data[0] = listOfUsers->validLogin(args[1], args[2]) ? '1' : '0';
		send(new_sd, data, 1, 0);
	}
	//marks user as having an active session
    //1: username
	else if(args[0] == "logMeIn"){
		session->joinSession(args[1]);
        send(new_sd, data, 0, 0);
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
        if(validateRegistration(args[1], args[2])){
            listOfUsers->addUser(args[1], args[2]);
            data[0] = '1';
        }
        else
            data[0] = '0';
        send(new_sd, data, 1, 0);
	}
	//removes a user from the session map
    //1: username
	else if(args[0] == "logout"){
		session->leaveSession(args[1]);
	}
    else
        return;
    close(new_sd);
}

//continuation of above
void logicPath2(int new_sd, vector<string> args, surveyMap* listOfSurveys, userMap* listOfUsers, sessionMap* session){
    char data[1];
    
    if(args[0] == "deleteAccount"){
        listOfUsers->removeUser(args[1]);
    }
    //returns concatinated string of all existing survey names
    else if(args[0] == "surveyNames"){
        send(new_sd, listOfSurveys->getSurveyNames().c_str(), listOfSurveys->getSurveyNames().size(), 0);
    }
    //sends the string description of a particular survey with \n delimiter
    //if survey doesn't exist, don't do anything.
    //1: survey name
    else if(args[0] == "getSurvey"){
        surveyData* surveyName = listOfSurveys->getSurvey(args[1]);
        if(surveyName)
            //for some reason, getting a lock on toString() causes this function to exit here without
            //reaching the returns at the end.
            send(new_sd, surveyName->toString().c_str(), surveyName->toString().size(), 0);
        else{
            data[0] = '0';
            send(new_sd, data, 1, 0);
        }
    }
    //creates a new survey, responds with failbit if invalid characters used
    //1: survey creator's username; 2: survey details
    else if(args[0] == "createSurvey"){
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
    else
        return;
    close(new_sd);
}

void socketMess(){
	//communication vars
	int new_sd;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
    vector<string> args(6);

    //Use these classes as containers for shared resources.
	sessionMap* session = new sessionMap();
	userMap* listOfUsers = new userMap();
	surveyMap* listOfSurveys = new surveyMap();
    //this manages saving/loading the contents of the userMap and surveyMap to/form files
    fileManager* fm = new fileManager(listOfUsers, listOfSurveys, "listOfUsers.txt", "listOfSurveys.txt");

    
    cout << "populating users and surveys...\n";
    fm->load();
    cout << "done!\n";
    
    //Startng saver in the background
    thread s(saver, fm, 5);
    s.detach();
    
    //Starting CLI
    //thread cli(CLI);
    //cli.detach();
    
    connection conn;
    conn.server(nullptr, "1234");
    int status = conn.getStatus();
    cout << "Listening for connection..." << endl;
    status = listen(conn.getSocketfd(), 5);
    if (status == -1)  cout << "listen error" << endl;
    
    while(true){
        cout << "about to accept" << endl;
		addr_size = sizeof(their_addr);
        //program sometimes hangs on accept because of SIGSTOP, this occurs if new_sd is
        //not closed, such as if either logic functions exit early. For some reason this
        //occured on a valid getSurvey request because of the lock_guard on the toString
        //method.
		new_sd = accept(conn.getSocketfd(), (struct sockaddr *)&their_addr, &addr_size);
		if (new_sd == -1)
		{
		    cout << "listen error" << endl;
		}
		else
		{
		    cout << "Connection accepted. Using new socketfd : "  <<  new_sd << endl;
		}
        
        /*
         Funny story about this. I had, like many people who answer unsurely a question
         on an exam, initially detached my threads as you had suggested. However, when I would
         run my code it would give me garbage messages in the command line. I assumed that
         this must mean something was wrong with simply detaching (even though I couldn't think
         of why) and came up with that ring-like structure, which was tbh kinda cool imo. 
         Anyway, turns out my initial intuition was correct and iostream just isn't thread-safe.
         nice memes
         */
        
        //let threads run asynchronously in the background. Split logic function into two
        //to increase concurrency. Threads that go down the "wrong" path are quickly destructed
        //as the logic function that doesnt contain the task will exit performing no tasks.
        args = reciever(new_sd);
        thread l1(logicPath1, new_sd, ref(args), listOfSurveys, listOfUsers, session);
        thread l2(logicPath2, new_sd, args, listOfSurveys, listOfUsers, session);
        l1.detach(); l2.detach();
    }
}

int main(){
	socketMess();
}

















