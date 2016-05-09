#ifndef CONNECTION_H_1234
#define CONNECTION_H_1234 1

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace backend{
class connection{
public:
    connection(const std::string& ip = "", const std::string& port = "1234");
	int getSocketfd()const;
	void setSocketfd(int newfd);
	int getStatus()const;
	void setStatus(int newStatus);
	addrinfo getHostInfo()const;
	addrinfo* getHostInfoList()const;
private:
	int status, socketfd, yes;
	std::string ip, port;
	struct addrinfo host_info;
	struct addrinfo *host_info_list; 
};
}
#endif