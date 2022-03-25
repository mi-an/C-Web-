#pragma once
#include <pthread.h>
#include <cstdio>
#include "noncopyable.h"


class MutexLock : noncopyable
{
public:
	MutexLock() { pthread_mutex_init(&mutex,NULL);}
	~MutexLock(){
		pthread_mutex_lock();
		//上的了锁，说明其他线程没有获取锁，可以析构
		pthread_mutex_destroy();
	}
	void lock() { pthread_mutex_lock(&mutex); }
	void unlock() { pthread_mutex_unlock(&mutex); }
	pthread_mutex_t* get() { return &mutex; }
private:
	pthread_mutex_t mutex;
private:
	frinend class Condition;
};

class MutexLockGuard : noncopyable
{
public:
	explicit MutexLockGuard(MutexLock& _mutex)
		:mutex(_mutex)
	{ mutex.lock(); }
	~MutexLockGuard()
	{ mutex.unlock(); }
private:
	MutexLock& mutex;
};

//使用RAII封装互斥锁，防止线程退出没有释放互斥锁，导致死锁现象
//MutexLock类 和 MutexLockGuard类的关系是关联的关系
//MutexLockGuard只是引用了一个MutexLock对象，不管理mutex对象生命周期



