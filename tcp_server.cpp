#include <boost/bind.hpp>
#include "tcp_server.h"
#include "common.hpp"
#include <iostream>

TcpServer::TcpServer(unsigned port, ReadHandler handler, int thread_num)
	:_h(handler),
	_loop(Creator<EventLoop>::Create()),
	_sock(-1),
	_port(port)
{
}

int TcpServer::do_handle(int sockfd)
{
	return _h(sockfd);
}

int TcpServer::do_accept()
{
	IpAddress ipaddr;
	//bzero(&ipaddr, sizeof(ipaddr));
	int connfd = _sock.accept(ipaddr);
	std::string s = ipaddr.toString();
	std::cout << "accept from " << s.c_str() << std::endl;
	SapperPtr sapper = Creator<Sapper>::Create(connfd, _loop);
	sapper->focusRead();
	Sapper::CallBackHandler cb = boost::bind(&TcpServer::do_handle, this, connfd);
	sapper->setReadHandler(cb);
	return 0;
}

void TcpServer::start()
{
	IpAddress ipaddr;
	ipaddr.setIp();
	ipaddr.setPort(_port);
	_sock.setReuseAddr(true);
	_sock.bindAddress(ipaddr);
	_sock.listen(128);
	SapperPtr sapper = Creator<Sapper>::Create(_sock.fd(), _loop);
	sapper->focusRead();
	Sapper::CallBackHandler cb = boost::bind(&TcpServer::do_accept, this);
	sapper->setReadHandler(cb);
	_loop->init();
	_loop->loop();

}
