#include "socket_server.h"
#include <sys/eventfd.h>
//#include "epoller.h"
#include "common.hpp"
#include "socket_util.h"
#include <sys/types.h>
//#include <boost/bind.hpp>
#include "socket_util.h"
#include <string>
#include <string.h>
#include <iostream>

#define TIMER_DEFAULT_INTERNAL 20
#define TIMER_INFINITE_INTERNAL -1


SocketServer::SocketServer(const Socket& socket)
	:_quit(true),
	_is_init(false),
	_timeout(TIMER_INFINITE_INTERNAL),
	_socket(socket),
	_accept_loop(std::make_shared<EventLoop>()),
	_io_loop(std::make_shared<EventLoop>()),
	_accept_th(std::shared_ptr<std::thread>()),
	_io_th(std::shared_ptr<std::thread>())
{

}

void SocketServer::set_handle(std::shared_ptr<BaseHandle> handle)
{
	_io_loop->setHandle(handle);
	_io_loop->setHandleActive(true);
}

void SocketServer::set_read_handle(ReadFunctor functor)
{
	_socket_read_cb = functor;
}

void SocketServer::set_new_connection_handle(NewConnectionFunctor functor)
{
	_socket_new_cb = functor;
}

void SocketServer::set_close_handle(CloseFunctor functor)
{
	_close_cb = functor;
}

void SocketServer::set_write_handle(WriteFunctor functor)
{
	_write_cb = functor;
}

int SocketServer::handle_accept()
{
	IpAddress ipaddr(1);
	bzero(&ipaddr,sizeof(ipaddr));
	int connfd = _socket.accept(ipaddr);
	SapperPtr sapper = Creator<Sapper>::Create(connfd, _io_loop);

	if (_socket_new_cb)
	{
		_socket_new_cb(sapper);
	}

	Sapper::CallBackHandler cbr = std::bind(&SocketServer::handle_receive, this, sapper);
	sapper->setReadHandler(cbr);
	sapper->focusRead();

	return 0;
}

int SocketServer::handle_receive(SapperPtr sapper)
{
	std::cout << "data arrive, fd is " << sapper->fd() << std::endl;
	ssize_t len = SocketUtil::Read(sapper->fd(), sapper->GetBuffer(), sapper->GetBufferLength());
	if (len > 0)
	{
		sapper->make(len);
		if (_socket_read_cb)
		{
			//SimpleBuffer buffer = std::make_shared<std::string>(sapper->receiveBuffer(), sapper->receiveLength());
			SimpleBuffer buffer = std::make_shared<std::string>();
			sapper->readBuffer(*buffer.get(), len);
			_socket_read_cb(sapper, buffer);
			sapper->setPostRead(false);
		}
		else 
		{
			sapper->setPostRead(true);
		}
	}
	if (len <= 0)
	{
		if (_close_cb)
		{
			_close_cb(sapper);
		}
		sapper->resetREvent();
		_io_loop->removeSapper(sapper);
		SocketUtil::Close(sapper->fd());
	}
	///-----
	//std::string str((char*)&buff);
	//std::cout << "data is : " << str << std::endl;
	return 0;
}

int SocketServer::handle_write(SapperPtr sapper, BufferPtr buff)
{
	std::cout << "data send, fd is " << sapper->fd() << std::endl;
	ssize_t len = SocketUtil::Write(sapper->fd(), buff.data(), buff.length());
	if (len > 0)
	{
		if (len == buff.length())
		{
			if (_write_cb)
			{
				sapper->cancelWrite();
				_write_cb(sapper, 0);
			}
		}
		else if (len < buff.length())
		{
			//没写完
			//send(sapper, buff.data() + len, buff.length() - len);
			BufferPtr ptr(((char*)buff.data() + len), buff.length() - len);
			sapper->setWriteHandler(std::bind(&SocketServer::handle_write, this, sapper, ptr));
		}
	}
	else 
	{
		//something wrong...
		if (_close_cb)
		{
			_close_cb(sapper);
		}
		sapper->loopPtr()->removeSapper(sapper);
		SocketUtil::Close(sapper->fd());
	}
	return 0;
}

void SocketServer::send(SapperPtr sapper, const char* buff, size_t len)
{
	if (len == 0)
	{
		return ;
	}
	BufferPtr ptr(buff, len);
	sapper->setWriteHandler(std::bind(&SocketServer::handle_write, this, sapper, ptr));
	sapper->focusWrite();
	//sapper->loopPtr()->wakeUp();
}

void SocketServer::init()
{
	_socket.listen(128);
	SapperPtr sapptor = Creator<Sapper>::Create(_socket.fd(), _accept_loop);
	sapptor->focusRead();
	sapptor->setReadHandler(std::bind(&SocketServer::handle_accept, this));

}

void SocketServer::async_start()
{
	init();
	if (!_accept_th)
	{
		_accept_th = std::make_shared<std::thread>(&SocketServer::start, this, _accept_loop);
	}
	if (!_io_th)
	{
		_io_th = std::make_shared<std::thread>(&SocketServer::start, this, _io_loop);
		if (_socket_read_cb)
		{
			_io_loop->setHandleActive(false);//开发人员为socket读事件设置了独立的处理函数，不需要handle处理了
		}
	}
	
}

void SocketServer::start(std::shared_ptr<EventLoop> loop)
{
	loop->loop();
}

void SocketServer::stop()
{
	_quit = true;
}

