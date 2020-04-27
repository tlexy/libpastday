#ifndef SOCKET_H
#define SOCKET_H

#include "ip_address.h"
#include <arpa/inet.h>
#include <netinet/in.h>

#define CREATE_NEW -1

class Socket
{
public:
	Socket(int sock = CREATE_NEW, bool nonblock = true);
	void bindAddress(const IpAddress&);
	void bindAddress(sockaddr* sa, socklen_t len);
	void listen(int maxfd);
	void connect();
	int accept(IpAddress&);
	int accept(sockaddr* r_sa, socklen_t* r_salen);
	int send(const void* buf, int nbytes);
	int recv(void* buf, int nbytes);
	int fd() { return _sockfd; }
	IpAddress address() { return _addr; }
	void setReuseAddr(bool flag);
private:
	int _sockfd;
	IpAddress _addr;
	bool _flag;//nonblock flag;
};

#endif
