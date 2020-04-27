#include "acceptor.h"

Acceptor::Acceptor(unsigned short int port, bool flag, uint64_t ipv4)
	:_ipaddr(port)
{
	_ipaddr.setIp();
	//_ipaddr.setPort(port);
	_sock.bindAddress(_ipaddr);
	_sock.setReuseAddr(true);
}

void Acceptor::listen(int max)
{
	_sock.listen(max);
}

int Acceptor::accept(IpAddress& ipaddr)
{
	//bzero(&ipaddr, sizeof(ipaddr);
	return _sock.accept(ipaddr);
}
