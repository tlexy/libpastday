#include "socket_util.h"
#include "socket.h"
#include <string.h>
#include <iostream>
#include "socket_server.h"

#include "base_handle.h"

class EchoHandle : public BinaryPacketHandle
{
public:
	EchoHandle(std::shared_ptr<SocketServer> server)
		:_server(server)
	{}

	virtual size_t getPacket(BufferPtr buffer)
	{
		return buffer.length();
	}

    virtual void handlePacket(const std::vector<BaseSession>& packets)
	{
		for (int i = 0; i < packets.size(); ++i)
		{
			std::cout << "fd: " << packets[i].sapper->fd() << ", receive data: " << packets[i].buffer->c_str() << std::endl;
			_server->send(packets[i].sapper, packets[i].buffer);
		}
	}

private:
	std::shared_ptr<SocketServer> _server;
};

void on_new_connection(SapperPtr sapper)
{
	std::cout << "new connection here..." << std::endl;
}

void on_close(SapperPtr sapper)
{
	std::cout << "sapper close: " << sapper->fd() << std::endl;
}

void on_write(SapperPtr sapper, int status)
{
	std::cout << "sapper write over: " << sapper->fd()  << "\t status =" << status << std::endl;
}

int main()
{
	Socket sock(-1);
	
	IpAddress ip_addr(5657);
	sock.bindAddress(ip_addr);
	sock.setReuseAddr(true);

	std::shared_ptr<SocketServer> server = std::make_shared<SocketServer>(sock);

	std::shared_ptr<BaseHandle> handle = std::make_shared<EchoHandle>(server);

	server->set_handle(handle);

	server->set_new_connection_handle(on_new_connection);
	server->set_close_handle(on_close);
	server->set_write_handle(on_write);

	server->async_start();

	std::cin.get();

	return 0;
}
