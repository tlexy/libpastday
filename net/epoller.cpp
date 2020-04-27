#include "epoller.h"
#include <sys/eventfd.h>
#include "sapper.h"
#include <iostream>


int EPoller::_eventSize = 8;

EPoller::EPoller()
	:Poller(),
	_epoll_fd(epoll_create1(EPOLL_CLOEXEC)),
	_events(_eventSize)
{
}

void EPoller::updateSapper(SapperPtr sapper)
{
	struct epoll_event event;
	event.events = sapper->focusEvents();
	event.data.fd = sapper->fd();

	if (_sappers.find(sapper->fd()) != _sappers.end())
	{
		//modify 
		epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, sapper->fd(), &event);

	}
	else
	{
		epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, sapper->fd(), &event);
		_sappers[sapper->fd()] = sapper;
		//new
	}
}

void EPoller::removeSapper(SapperPtr sapper)
{
	if (_sappers.find(sapper->fd()) != _sappers.end())
	{
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, sapper->fd(), NULL);
		_sappers.erase(sapper->fd());
		if (_sappers.size()*2 <= _events.size())
		{
			_events.resize(_sappers.size());
			_eventSize = _sappers.size();
		}
	}
}

void EPoller::poll(int timeout, SapperList& activeSapper)
{
	int n = epoll_wait(_epoll_fd, &*_events.begin(), static_cast<int>(_events.size()), timeout);
	if (n == _events.size())
	{
		_eventSize *= 2;
		_events.resize(_eventSize);
	}
	for (int i = 0; i < n; ++i)
	{
		SapperPtrMap::iterator it = _sappers.find(_events[i].data.fd);
		if (it != _sappers.end())
		{
			it->second->setREvent(_events[i].events);
			activeSapper.push_back(it->second);
		}
	}
}

