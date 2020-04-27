#ifndef SAPPER_H
#define SAPPER_H

#include <vector>
#include <memory>
#include <functional>
#include "queue_buffer.h"
#include <string>


class EventLoop;
class Sapper;

typedef std::shared_ptr<EventLoop> EventLoopPtr;

namespace SAPPER
{
	enum res_code
	{
		OK = 0,
		OVER = 1,
	};
}

class Sapper 
	: public std::enable_shared_from_this<Sapper>
{
public:
	typedef std::function<int()> CallBackHandler;
	Sapper(int sockfd, EventLoopPtr, size_t init_size = 1024, size_t max_package_size = 4096);
	int focusEvents() { return _events; }
	int revents() { return _revents; }
	void focusRead();
	void focusWrite();
	void cancelWrite();
	void setREvent(int events);
	void resetREvent();
	bool isReadEvent();
	bool isPostRead();
	void setPostRead(bool flag);
	void setReadHandler(const CallBackHandler& cb) { _readCallback = cb; }
	void setWriteHandler(const CallBackHandler& cb) { _writeCallback = cb; }
	int handleEvent();
	int fd() { return _fd; }
	EventLoopPtr loopPtr() { return _loop; }

	void make(size_t len);
	void* GetBuffer();
	size_t GetBufferLength();

	const void* receiveBuffer();
	size_t receiveLength();
	void readBuffer(std::string&, size_t len);
private:
	int _events;
	int _revents;
	int _fd;
	bool _is_post_read;
	EventLoopPtr _loop;
	CallBackHandler _readCallback;
	CallBackHandler _writeCallback;
	static const int _read_event;
	static const int _write_event;
	static const int _none_event;

	size_t _max_package_size;

	std::shared_ptr<QueueBuffer> _buffptr;
private:
	void update();
};


#endif
