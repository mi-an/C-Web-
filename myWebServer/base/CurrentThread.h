#pragma once
#include <stdint.h>

namespace CurrentThread
{
	extern __thread int t_cachedTid;
	extern __thread char t_tidString[32];
	extern __thread int t_tidStringLength;
	extern __thread const char* t_threadName;
	void cachedTid();
	inline int tid()
	{
		if(t_cachedTid == 0)
		{
			cachedTid();
		}
		return t_cachedTid;
	}

	inline const char* tidString()
	{
		return t_tidString;
	}

	inline int tidStringLength()
	{
		return t_tidStringLength;
	}
	inline const char* name() { return t_cachedName; }
}
