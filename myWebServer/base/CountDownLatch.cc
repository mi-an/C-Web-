#include "CountDownLatch.h"


CountDownLatch::CountDownLatch(int count)
	:mutex_(),
	condition_(mutex_),
	count_(count)
{}

void CountDownLatch::wait()
{
	MutexLockGuard mutex(mutex_);
	while(count_>0)
	{
		cond_.wait();
	}
}

void CountDownLatch::countDown()
{
	MutexLockGuard mutex(mutex_);
	--count_;
	if(count_==0)
	{
		cond_.notifyAll();
	}
}



