#include "sapper.h"
#include "event_loop.h"
#include <sys/poll.h>
#include <iostream>


const int Sapper::_none_event = 0;
const int Sapper::_read_event = POLLIN | POLLPRI;
const int Sapper::_write_event = POLLOUT;

Sapper::Sapper(int sockfd, EventLoopPtr loop, size_t init_size, size_t max_package_size)
	:_fd(sockfd),
	_events(0),
	_revents(0),
	_loop(loop),
	_buffptr(std::make_shared<QueueBuffer>(init_size)),
	_max_package_size(max_package_size)
{
}

void Sapper::setREvent(int events)
{
	_revents = events;
}

bool Sapper::isReadEvent()
{
	return (_revents & _read_event);
}

bool Sapper::isPostRead()
{
	return _is_post_read;
}

void Sapper::setPostRead(bool flag)
{
	_is_post_read = true;
}

void Sapper::resetREvent()
{
	_revents = 0;
}

void Sapper::focusRead()
{
	_events |= _read_event;
	update();
}

void Sapper::focusWrite()
{
	_events |= _write_event;
	update();
}

void Sapper::cancelWrite()
{
	_events ^= _write_event;
	update();
}

int Sapper::handleEvent()
{
	//std::cout << "events :" << _revents << std::endl;
	//std::cout << "revent :" << _read_event << std::endl;
	//std::cout << "wevent :" << _write_event << std::endl;
	if ((_revents & _read_event) && _readCallback)
	{
		return _readCallback();
	}
	if ((_revents & _write_event) && _writeCallback)
	{
		return _writeCallback();
	}
	//_revents = 0;
	return 0;
}

void Sapper::update()
{
	if (_loop)
	{
		_loop->updateSapper(shared_from_this());
	}
}

void Sapper::make(size_t len)
{
	_buffptr->hasWritten(len);
}

void* Sapper::GetBuffer()
{
	//保证buffer至少能够接收一个包
	_buffptr->ensureWritableBytes(_max_package_size);
	return _buffptr->beginWrite();
}

size_t Sapper::GetBufferLength()
{
	return _buffptr->usableBytes();
}

const void* Sapper::receiveBuffer()
{
	_buffptr->peek();
}

size_t Sapper::receiveLength()
{
	return _buffptr->readableBytes();
}

void Sapper::readBuffer(std::string& contents, size_t len)
{
	_buffptr->retrieve(len, contents);
}

