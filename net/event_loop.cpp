#include "event_loop.h"
#include <sys/eventfd.h>
#include "sapper.h"
#include "epoller.h"
#include "../base/common.hpp"
#include "socket_util.h"
#include <sys/types.h>
#include "base_handle.h"

int EventLoop::_timeout = 10000;

EventLoop::EventLoop()
	:_wakeup_fd(eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
	_quit(true),
	_looping(false),
	_poller(Creator<EPoller>::Create()),
	_handle(std::shared_ptr<BaseHandle>()),
	_is_handle_active(false)
{
	//init();
}

void EventLoop::init()
{
	SapperPtr sapper = Creator<Sapper>::Create(_wakeup_fd, shared_from_this());
	sapper->focusRead();
	sapper->setReadHandler(std::bind(&SocketUtil::Read, _wakeup_fd, &_wake_read, sizeof(_wake_read)));

}

void EventLoop::loop()
{
	init();
	//
	_quit = false;
	_looping = true;
	while (!_quit && _poller)
	{
		_activeSapper.clear();
		_poller->poll(_timeout, _activeSapper);
		
		for (int i = 0; i < _activeSapper.size(); ++i)
		{
			int res = _activeSapper[i]->handleEvent();
		}
		if (_is_handle_active && _handle)
		{
			_handle->handle(_activeSapper);
		}
		std::vector<Functor> functors;
		{
			MutexGuard lock(_mutex);
			functors.swap(_pendingFunc);
		}
		for (int i = 0; i < functors.size(); ++i)
		{
			functors[i]();
		}
	}
	_looping = false;
}

void EventLoop::setHandle(std::shared_ptr<BaseHandle> handle)
{
	_handle = handle;
}

void EventLoop::setHandleActive(bool flag)
{
	_is_handle_active = true;
}

void EventLoop::stop()
{
	_quit = true;
	EventLoop::wakeUp();
}

void EventLoop::wakeUp()
{
	uint64_t a = 1;
	SocketUtil::Write(_wakeup_fd, &a, sizeof(a));
}

void EventLoop::updateSapper(SapperPtr sapper)
{
	_poller->updateSapper(sapper);
}

void EventLoop::removeSapper(SapperPtr sapper)
{
	_poller->removeSapper(sapper);
}

void EventLoop::runAfterWake(const Functor& func)
{
	MutexGuard lock(_mutex);
	_pendingFunc.push_back(func);
}

void EventLoop::breakAndRun(const Functor& func)
{
	MutexGuard lock(_mutex);
	_pendingFunc.push_back(func);
	wakeUp();
}
