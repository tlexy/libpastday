#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "socket.h"
#include "ip_address.h"

class Acceptor
{
public:
	Acceptor(unsigned short int port, bool nonblock = true, uint64_t ipv4 = INADDR_ANY);
	void listen(int max);
	int accept(IpAddress&);
	int fd() { return _sock.fd(); }
private:
	Socket _sock;
	IpAddress _ipaddr;
};

#endif
