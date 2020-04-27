#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <vector>
#include <memory>
#include <functional>
#include <vector>
#include "../base/mutex.hpp"
#include "epoller.h"

class BaseHandle;

class EventLoop : public std::enable_shared_from_this<EventLoop>
{
public:
	typedef std::function<void()> Functor;
	EventLoop();
	virtual void updateSapper(SapperPtr);
	virtual void removeSapper(SapperPtr);
	void init();
	virtual void loop();
	void runAfterWake(const Functor&);
	void breakAndRun(const Functor&);
	void wakeUp();
	void stop();
	void setHandle(std::shared_ptr<BaseHandle>);
	void setHandleActive(bool flag);

protected:
	int _wakeup_fd;
	uint64_t _wake_read;
	SapperList _activeSapper;
	bool _quit;
	bool _looping;
	static int _timeout;
	PollerPtr _poller;
	std::vector<Functor> _pendingFunc;
	Mutex _mutex;
	std::shared_ptr<BaseHandle> _handle;
	bool _is_handle_active;
	
};

#endif
