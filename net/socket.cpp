#include "socket_util.h"
#include "socket.h"
#include <netinet/in.h>

Socket::Socket(int sock, bool flag)
	:_sockfd(sock),
	_flag(flag),
	_addr(1)
{
	if (_sockfd == CREATE_NEW)
	{
		_sockfd = SocketUtil::Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, flag);
	}
}

void Socket::bindAddress(const IpAddress& addr)
{
	_addr = addr;
}

void Socket::bindAddress(sockaddr* sa, socklen_t len)
{
	_addr = IpAddress::fromRawSocketAddress(sa, len);
}

void Socket::listen(int max)
{
	SocketUtil::Bind(_sockfd, _addr);
	SocketUtil::Listen(_sockfd, max);
}

void Socket::connect()
{
	SocketUtil::Connect(_sockfd, _addr);
}

int Socket::accept(IpAddress& ipaddr)
{
	return SocketUtil::Accept(_sockfd, ipaddr, _flag);
}

int Socket::accept(sockaddr* r_sa, socklen_t* r_salen)
{
	return SocketUtil::Accept(_sockfd, r_sa, r_salen, _flag);
}

int Socket::send(const void* buf, int nbytes)
{
	return SocketUtil::Write(_sockfd, buf, nbytes);
}

int Socket::recv(void* buf, int nbytes)
{
	return SocketUtil::Read(_sockfd, buf, nbytes);
}

void Socket::setReuseAddr(bool flag)
{
	int opt = flag ? 1 : 0;
	setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
}

