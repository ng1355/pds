#include "connection.h"

#include <iostream>
#include <string> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace backend{
	connection::connection(const std::string& ip, const std::string& port): yes(1), ip(ip), port(port){
		//std::cout << "building connection" << std::endl;
		memset(&host_info, 0, sizeof host_info);
		//std::cout << "Setting up structs" << std::endl;
		host_info.ai_family = AF_UNSPEC;
		host_info.ai_socktype = SOCK_STREAM;
		host_info.ai_flags = AI_PASSIVE;
		status = getaddrinfo(NULL, "1234", &host_info, &host_info_list);
	    if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;

		std::cout << "creating socket..." << std::endl;
		socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
	    if (socketfd == -1)  std::cout << "socket error " << std::endl;	

		std::cout << "Binding socket..." << std::endl;
		status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	    if (status == -1)  std::cout << "bind error" << std::endl;
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