
#include "socket_util.h"
#include "socket.h"
#include <string.h>
#include <iostream>
#include "socket_server.h"

void on_new_connection(SapperPtr sapper)
{
	std::cout << "new connection here..." << std::endl;
}

void on_read(SapperPtr sapper, SimpleBuffer buffer)
{
	std::cout << "recv data :" << buffer->c_str() << std::endl;
}

void on_close(SapperPtr sapper)
{
	std::cout << "connection close" << std::endl;
}

int main()
{
	Socket sock(-1);
	
	IpAddress ip_addr(5656);
	sock.bindAddress(ip_addr);
	sock.setReuseAddr(true);

	SocketServer server(sock);
	server.set_new_connection_handle(on_new_connection);
	server.set_read_handle(on_read);
	server.set_close_handle(on_close);

	server.async_start();

	std::cin.get();

	return 0;
}
