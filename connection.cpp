#include "connection.h"

#include <iostream>
#include <string> 
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

namespace backend{
	connection::connection(): yes(1), ip(ip), port(port){}

    
    void connection::server(const char*, const char* port){
    //std::cout << "building connection" << std::endl;
    memset(&host_info, 0, sizeof host_info);
    //std::cout << "Setting up structs" << std::endl;
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;
    status = getaddrinfo(NULL, port, &host_info, &host_info_list);
    if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;
    
    std::cout << "creating socket..." << std::endl;
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socketfd == -1)  std::cout << "socket error " << std::endl;
    
    std::cout << "Binding socket..." << std::endl;
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)  std::cout << "bind error" << std::endl;
    }
    
    ssize_t connection::accept(){
        return ::accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    }
    
    int connection::listen(unsigned num){
        return ::listen(socketfd, num);
    }
    
    //Opens a temporary channel for sending and recieving as a client
    void connection::send_and_recv(const char* port, std::string& data, int len){
        string container;
        client_host_info.ai_family = AF_UNSPEC;
        client_host_info.ai_socktype = SOCK_STREAM;

        status = getaddrinfo(nullptr, port, &client_host_info, &client_host_info_list);
        
        ssize_t bytesSent;
        std::cout << "Creating a socket..."  << std::endl;
        int socketfd ; // The socket descripter
        socketfd = socket(client_host_info_list->ai_family, client_host_info_list->ai_socktype,
                          client_host_info_list->ai_protocol);
        if (socketfd == -1)  std::cout << "socket error " ;
        
        std::cout << "Connect()ing..."  << std::endl;
        status = connect(socketfd, client_host_info_list->ai_addr, client_host_info_list->ai_addrlen);
        if (status == -1)  std::cout << "connect error" ;
        
        bytesSent = send(socketfd, data.c_str(), len, 0);
        
        std::cout << "Waiting to recieve data..."  << std::endl;
        ssize_t bytes_recieved;
        char incomming_data_buffer[1000]; //good enough for small scale stuff
        bytes_recieved = recv(socketfd, incomming_data_buffer,1000, 0);
        // If no data arrives, the program will just wait here until some data arrives.
        if (bytes_recieved == 0) std::cout << "host shut down." << std::endl ;
        if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
        std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
        incomming_data_buffer[bytes_recieved] = '\0' ;
        std::cout << incomming_data_buffer << std::endl;
        std::cout << "Receiving complete. Closing socket..." << std::endl;
        freeaddrinfo(host_info_list);
        close(socketfd);
    }
    
	int connection::getSocketfd()const{
		return socketfd;
	}

	void connection::setSocketfd(int newfd){
		socketfd = newfd;
	}

	int connection::getStatus()const{
		return status;
	}

	void connection::setStatus(int newStatus){
		status = newStatus;
	}

	addrinfo connection::getHostInfo()const{
		return host_info;
	}

	addrinfo* connection::getHostInfoList()const{
		return host_info_list;
	}
}