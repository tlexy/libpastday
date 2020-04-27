#ifndef COMMON_H
#define COMMON_H

#include <memory>

template<typename TYPE>
class Creator
{
public:
	typedef std::shared_ptr<TYPE> PTR;

	static PTR Create()
	{
		return PTR(new TYPE());
	}

	template<typename arg1>
	static PTR Create(arg1 a1)
	{
		return PTR(new TYPE(a1));
	}

	template<typename arg1, typename arg2>
	static PTR Create(arg1 a1, arg2 a2)
	{
		return PTR(new TYPE(a1, a2));
	}
};


#endif
