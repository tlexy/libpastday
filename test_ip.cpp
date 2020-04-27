#include <iostream>
#include "ip_address.h"

int main()
{
	IpAddress ip;
//	ip.setIp(0xffffffff);
	ip.setIp(std::string("114.114.114.114"));
	ip.setPort(20000);
	std::cout << "ip is " << ip.toString() << std::endl;

	return 0;
}
