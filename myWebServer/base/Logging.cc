#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging* AsyncLogger_;

std::string Logger::logFileName_ = "./WebServer.log";

//异步线程初始化
void once_init()
{
	AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
	AsyncLogger_->start();
}

//打印,每次打印日志都会调用它
void output(const char* msg,int len)
{
	pthread_once(&once_control_,once_init);
	//但是异步日志类只需要初始化一次，这个函数可以让once_init
	//函数只调用一次 
	AsyncLogger_->append(msg,len);
}

Logger::Impl::Impl(const char* fileName,int line)
	: stream_(), //初始化LogStream对象
	  line_(line),
	  basename_(fileName)
{
	formatTime();//会输出时间
}

//格式化时间，最后输出
void Logger::Impl::formatTime()
{
	struct timeval tv;
	time_t time;
	char str_t[26] = {0};
	gettimeofday(&tv,NULL);
	time = tv.tv_sec;
	struct tm* p_time = localtime(&time);
	strftime(str_t,26,"%Y-%m-%d %H:%M:%S\n",p_time);
	stream_ << str_t;
}


Logger::Logger(const char* fileName,int line)
	: impl_(fileName,line)
{}

Logger::~Logger()
{
	impl_.stream_ << " -- " << impl_.basename_ << ":" << impl_.line_ << '\n';
	const LogStream::Buffer& buf(stream().buffer());//对LogStream中缓冲区的引用
	output(buf.data(),buf.length());//输出到异步日志的缓冲区
}



/*


Logger::~Logger()
{
	impl_.finish();
	const LogStream::Buffer& buf(stream().buffer());//buf是缓冲区的引用
	g_output(buf.data(),buf.length());//对buf操作相当于对缓冲区操作
	//在g_output里面调用异步日志的append函数，完成日志线程得到日志收集
	if ( impl_.level_ == FATAL )
	{
		g_flush();
		abort();
	}
}


void defaultOutput(const char* msg,int len)
{
	size_t n = fwrite(msg,1,len,stdout);
	(void)n;
}

void defaultFlush()
{
	fflush(stdout);
}


Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

*/
