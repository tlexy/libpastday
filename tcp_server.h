#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "event_loop.h"
#include "sapper.h"
#include "socket_util.h"
#include "socket.h"
#include "common.hpp"
#include <string.h>

//specialize version of tcp server.
class TcpServer
{
public:
typedef boost::function<int(int)> ReadHandler;
	TcpServer(unsigned port, ReadHandler handler, int thread_num = 1);
	void start();

private:
	ReadHandler _h;
	EventLoopPtr _loop;
	Socket _sock;
	unsigned _port;

private:
	int do_accept();
	int do_handle(int);
};

#endif
