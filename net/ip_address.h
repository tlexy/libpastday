#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

/*
* currently only support IPv4, it also can be extended to support IPv6
*/

class IpAddress
{
public:
	IpAddress(unsigned short port, int family = AF_INET);
	void setIp(uint32_t ip = INADDR_ANY, int family = AF_INET);
	void setIp(const std::string&, int family = AF_INET);
	void setPort(unsigned short port);
	struct sockaddr* rawAddressPtr() { return (struct sockaddr*)&_addr; }
	std::string toString(bool withPort = true);
	int addressLength();
	int family() { return _addr.addr_ip4.sin_family; }
	static IpAddress fromRawSocketAddress(sockaddr*, socklen_t);
private:
	union
	{
		struct sockaddr_in addr_ip4;
		struct sockaddr_in6 addr_ip6;
	}_addr;
};

#endif
