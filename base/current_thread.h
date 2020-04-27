#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

//#include <unistd.h>

namespace CurrentThread
{
	extern __thread int _cacheTid;
	
	void doCacheTid();

	inline int tid()
	{
		if (_cacheTid == 0)
		{
			doCacheTid();
		}
		return _cacheTid;
	}

	void sleep(int second);
}

#endif
