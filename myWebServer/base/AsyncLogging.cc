#include "AsyncLogging.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include "LogFile.h"



AsyncLogging::AsyncLogging(const string logFileName_,int flushInterval)
	: flushInterval_(flushInterval),
	running_(false),
	basename_(logFileName_),
	thread_(std::bind(&AsyncLogging::threadFunc,this),"Logging"),
	latch_(1),
	mutex_(),
	cond_(mutex_),
	currentBuffer_(new Buffer),
	nextBuffer_(new Buffer),
	buffers_()
{
	assert(logFileName_.size() > 1);
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
}

//业务线程



void AsyncLogging::append(const char* logline,int len)
{
	MutexLockGuard lock(mutex_);
	//保护缓冲区，可能有多个线程向缓冲区中写入数据
	if(currentBuffer_->avail() > len)
	{
		//当前缓冲区没满，将数据追加到末尾
		currentBuffer_->append(logline,len);
	}
	else
	{
		//当前缓冲区已满，将当前缓冲区添加到待写入文件的已填满的缓冲区列表
		buffers_.push_back(currentBuffer_);//就指向空了
		currentBuffer_.reset();

		//将预备缓冲区设置成当前缓冲区
		if(nextBuffer_)
		{
			currentBuffer_ = std::move(nextBuffer_);
		}
		else
		{
			//这种情况：业务线程写入过快，一下把两块缓冲区都写完，
			//只好分配一块新的缓冲区,交给currentBuffer指针
			currentBuffer_.reset(new Buffer);
		}
		currentBuffer_->append(logline,len);
		cond_.notify();//通知日志线程开始写入日志
	}
}


//日志线程

void AsyncLogging::threadFunc()
{
	assert(running_ == true);
	latch_.countDown();
	LogFile output(basename_,rollSize_.false);//我们要写入的日志
	//线程一启动，就准备了两个空闲缓冲区
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	//用于接收buffers_,交换buffers_,然后处理bufferToWrite中的数据
	BufferVector bufferToWrite;
	bufferToWrite.reserve(16);
	while(running_)
	{
		assert(newBuffer1 && newBuffer1->length() == 0);
		assert(newBuffer2 && newBuffer2->length() == 0);
		assert(bufferToWrote.empty());

		{
			MutexLockGuard lock(mutex_);
			//条件变量的常规用法，这里常规的是while循环
			//那么为什么这里可以用if,if不能解决虚假唤醒的情况
			//但是这里，就算是被虚假唤醒的也无所谓，逻辑是没问题的
			if(buffers_.empty())//只有当列表为空是，才等待，等待列表不空
			{
				cond_.waitForSeconds(flushInterval_);
				//等待业务线程写满一个或多个缓冲区，或者超时时间到来
			}
			buffers_.push_back(currentBuffer_);
			currentBuffer_.reset();
			//将当前缓冲区移入buffers_
			currentBuffer_ = std::move(newBuffer1);
			//将空闲的newBuffer1设置成当前缓冲区
			buffersToWrite.swap(buffers_);
			//将写日志操作防止到临界区之外，缩小临界区范围，提高效率
			//同时可以让我们在临界区之外安全的写日志
			if(!nextBuffer_)
			{
				nextBuffer_ = std::move(newBuffer2);
				//确保至少有一个预备缓冲区可供调配，减少业务线程进行内存分配的概率
				//否则业务线程写入日志的效率会降低
			}
		}
		//下面开始写日志,不需要再临界区内
		assert(!bufferToWrite.empty());

		//写日志还要考虑一种异常情况，消息堆积
		//怎么发生的呢？业务线程陷入死循环，拼命发送日志消息，超过日志线程的处理能力
		//生产速度超过消费速度的问题，会造成数据在内存中堆积，严重是引发性能问题(可用内存不足
		//)，或程序崩溃(分配内存失败)
		if(buffersToWrite.size() > 25)//超过100M认为发生消息堆积
		{
			char buf[256];
			snprintf(buf,sizeof buf,"Dropped log messages at %s %zd larger buffers\n",
					Timestamp::now().toForwardString().c_str(),
					bufferToWrite.size()-2);
			fputs(buf,stderr);
			output.append(buf,static_cast<int>(strlen(buf)));
			buffersToWrite.erase(buffersToWrite.begin()+2,buffersToWrite.end());
			//丢掉多余日志，腾出内存，仅保留两块缓冲区
		}

		for(size_t i = 0;i< buffersToWrite.size();++i)
		{
			output.append(buffersToWrite[i].data(),buffersToWrite[i].length());
		}//把日志消息写入到logfile当中

		//再预留两块缓冲区交给newBuffer1和newBuffer2
		if(buffersToWrite.size() > 2)
		{
			buffersToWrite.resize(2);
		}
		if(!newBuffer1)
		{
			assert(!buffersToWrite.empty());
			newBuffer1 = buffersToWrite.back();
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}
		if(!newBuffer2)
		{
			assert(!buffersToWrite.empty());
			newBuffer2 = buffersToWrite.back();
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}
		
		buffersToWrite.clear();
		output.flush();//刷新到日志
	}
	output.flush();//刷新到日志
}


