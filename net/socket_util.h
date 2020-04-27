#ifndef SOCKET_UTILITY_H
#define SOCKET_UTILITY_H

#include <arpa/inet.h>
#include "ip_address.h"
#include <errno.h>
#include <fcntl.h>

namespace SocketUtil
{
	int Socket(int family = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP, bool nonblock = true);
	void Bind(int sockfd, struct sockaddr* sa, socklen_t salen);
	void Bind(int sockfd, IpAddress& ipaddr); 
	void Connect(int sockfd, struct sockaddr* sa, socklen_t salen);
	void Connect(int sockfd, IpAddress& ipaddr);
	int Accept(int sockfd, struct sockaddr* r_sa, socklen_t* r_salen, bool nonblockAndCloExec = true);
	int Accept(int sockfd, IpAddress& ipaddr, bool nonblockAndCloExec = true);
	void Listen(int sockfd, int maxN = SOMAXCONN);
	void ShutdownWrite(int sockfd);

	///////////////////////////////////////////

	ssize_t Read(int fd, void* buf, size_t nbytes);
	ssize_t Write(int fd, const void* buf, size_t nbytes);
	void Close(int fd);
}

#endif
