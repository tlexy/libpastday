#ifndef TIMER_EVENT_LOOP_H
#define TIMER_EVENT_LOOP_H
/**
	A event loop with timer functionality.
	author: Lin, mar,14,2018
*/
#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include "base/mutex.hpp"
#include "epoller.h"
#include "../base/common.h"
#include "socket.h"
#include "event_loop.h"
#include "simple_buffer.h"
#include <unordered_map>
#include "buffer_ptr.h"
#include "sapper.h"

typedef std::function<void(SapperPtr)> NewConnectionFunctor;
typedef std::function<void(SapperPtr)> CloseFunctor;
typedef std::function<void(SapperPtr, SimpleBuffer)> ReadFunctor;
typedef std::function<void(SapperPtr, int)> WriteFunctor; //第二个int是错误码

#define KB 1024*1024

class BaseHandle;

class SocketServer
{
public:
	SocketServer(const Socket& serverSocket);
	int handle_accept();
	int handle_receive(SapperPtr handle);
	int handle_write(SapperPtr handle, BufferPtr);

	void set_handle(std::shared_ptr<BaseHandle>);
	void set_new_connection_handle(NewConnectionFunctor);
	void set_read_handle(ReadFunctor);
	void set_close_handle(CloseFunctor);
	void set_write_handle(WriteFunctor);

	void send(SapperPtr, const char* buff, size_t len);
	void send(SapperPtr, SimpleBuffer);
	
	void async_start();
	void stop();

private:
	void async_write(SapperPtr, BufferPtr);

protected:
	void init();//Add event fd for wake up.

private:
	Socket _socket;
	//int _wakeup_fd;
	int _timeout;
	bool _is_init;
	bool _quit;
	std::shared_ptr<EventLoop> _accept_loop;
	std::shared_ptr<EventLoop> _io_loop;
	std::shared_ptr<std::thread> _accept_th;
	std::shared_ptr<std::thread> _io_th;

	ReadFunctor _socket_read_cb;
	NewConnectionFunctor _socket_new_cb;
	CloseFunctor _close_cb;
	WriteFunctor _write_cb;

	std::unordered_map<int, Sapper> _sappers;

	//char _buff[64*KB];

private:
	void start(std::shared_ptr<EventLoop>);

};

#endif
