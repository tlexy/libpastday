#ifndef _MUTEX_H
#define _MUTEX_H

#include <pthread.h>

class Mutex
{
public:
	Mutex()
	{
		if (pthread_mutex_init(&_mutex, NULL) != 0)
		{
			throw std::exception();
		}
	}

	~Mutex()
	{
		pthread_mutex_destroy(&_mutex);
	}

	bool lock()
	{
		return pthread_mutex_lock(&_mutex) == 0;
	}

	bool unlock()
	{
		return pthread_mutex_unlock(&_mutex) == 0;
	}

	pthread_mutex_t* rawMutexPtr()
	{
		return &_mutex;
	}

private:
	pthread_mutex_t _mutex;
};

class MutexGuard
{
public:
    MutexGuard(Mutex& mutex)
        :_mutex(mutex)
    {
        _mutex.lock();
    }

    ~MutexGuard()
    {
        _mutex.unlock();
    }
private:
    Mutex& _mutex;
};

#endif
