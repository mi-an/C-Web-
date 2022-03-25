#pragma once
#include <functional>
#include <string>
#include <vector>
#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"



class AsyncLogging : noncopyable
{
public:
	AsyncLogging(const string basename,
			int flushInterval = 2);
	~AsyncLogging()
	{
		if(running_)
		{
			stop();
		}
	}

	//供业务线程调用，将日志数据写入到缓冲区
	void append(const char* logline,int len);

	void start()
	{
		running_ = true;
		thread_.start();//日志线程启动,会调用threadFunc()函数
		latch_.wait();//等待thread_已经启动才会继续往下走
	}


	void stop()
	{
		running_ = false;
		cond_.notify();
		thread_.join();
	}
private:

	//日志线程调用，这个函数执行的就是将缓冲区中的数据写入到日志文件
	void threadFunc();

	typedef FixedBuffer<kLargeBuffer> Buffer;
	//缓冲区类型，是一块固定大小的缓冲区
	typedef vector<std::shared_ptr<Buffer>> BufferVector;//缓冲区列表
	typedef std::shared_ptr<Buffer> BufferPtr;
	
	
	const int flushInterval_;//超时时间，如果在超时时间内没写满缓冲区，也要将缓冲区添加到buffer_
	bool running_;
	std::string basename_;//日志文件的basename
	//size_t rollSize_;//滚动大小
	Thread thread_;//日志线程
	CountDownLatch latch_;//用于等待线程启动
	MutexLock mutex_;//互斥量
	Condition cond_;//条件变量


	BufferPtr currentBuffer_;//当前缓冲区
	BufferPtr nextBuffer_;//预备缓冲区
	BufferVector buffers_;//待写入文件的已填满的缓冲区,缓冲区指针列表
};
