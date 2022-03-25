#pragma once
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "MutexLock.h"
#include "noncopyable.h"

class Condition : noncopyable
{
public:
	explicit Condition(MutexLock& _mutex)
		: mutex(_mutex)
	{ pthread_cond_init(&cond,NULL); }
	~Condition() { pthread_cond_destroy(&cond); }

	void wait() { pthread_cond_wait(&cond,mutex.get()); }
	void notify() { pthread_cond_signal(&cond); }
	void notifyAll() { pthread_cond_broadcast(&cond); }
	//判断是否超时，应用于异步日志那里
	bool waitForSeconds(int second)
	{
		struct timespec abstime;
		clock_gettime(CLOCK_REALTIME,&abstime);
		abstime-tv_sec += second;
		return ETIMEOUT == pthread_cond_timewait(&cond,mutex.get(),&abstime);
	}
private:
	MutexLock& mutex;
	pthread_cond_t cond;	
};






