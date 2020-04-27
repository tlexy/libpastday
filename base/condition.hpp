#ifndef _CONDITION_H
#define _CONDITION_H

#include <exception>
#include <pthread.h>
#include "mutex.hpp"

class Condition
{
public:
	Condition(Mutex& mutex)
        :_mutex(mutex)
	{
		if (pthread_cond_init(&_cond, NULL) != 0)
		{
			throw std::exception();
		}
	}

	~Condition()
	{
		pthread_cond_destroy(&_cond);
	}

	bool wait()
	{
		//MutexGuard lock(_mutex);
		pthread_cond_wait(&_cond, _mutex.rawMutexPtr()) == 0;
	}

	bool signal()
	{
		return pthread_cond_signal(&_cond) == 0;
	}

private:
	Mutex& _mutex;
	pthread_cond_t _cond;
};

#endif
