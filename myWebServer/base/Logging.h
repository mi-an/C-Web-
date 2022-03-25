#pragma once
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "LogStream.h"

class AsyncLogging;

class Logger
{
public:
	Logger(const char* fileName,int line);
	~Logger();
	LogSteam &stream() { return impl_.stream_; }

	static void setLogFileName(std::string fileName) { LogFileName_ = fileName; }
	static std::string getLogFileName() { return logFileName_; }
private:
	class Impl
	{
	public:
		Impl(const char* fileName,int line);
		void formatTime();

		LogStream stream_;
		int line_;
		std::string basename_;
	};
	Impl impl_;
	static std::string logFilename_;
};

#define LOG Logger(__FILE__,__FILE__).stream()






/*
//日志级别
////TRACE:指出比DEBUG粒度更细的一些信息事件（开发过程中使用）
////DEBUG：指出细粒度信息事件对调试程序很有帮助
////INFO：表名消息在粗粒度级别上，突出强调应用程序的运行过程
////WARN：系统能正常运行，但可能存在潜在错误情形
////ERROR：指出虽然发生错误事件，但不影响运行
////FATAL：指出每个严重的错误事件，将会导致应用程序退出
//
//
////logger的使用时序图：
////首先构造一个logger对象，
////调用stream方法，返回一个logStream对象
////再调用logStream的运算符重载输出
//
//
////logger => Impl => logstream => operator<<FixedBuffer => g_output => g_flush
////logger是一个外部的日志类，定义了一些级别
////Impl是实际实现的类，比如如何格式化日志
////再借助logstream输出，先输出到缓冲区
////最后再输出到对应设备缓冲区，刷新才真正到达设备

class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,
	};

	//返回的就是impl对象的stream
	LogStream& stream() { return impl_.stream_; }

private:

class Impl
{
public:
	typedef Logger::LogLevel LogLevel;
	Impl(LogLevel level,int old_errno,const SourceFile& file,int line);
	void formatTime();
	void finish();
	
	Timestamp time_;
	LogStream stream_;
	LogLevel level_;
	int line;
	SourceFile basename_;

};

	Impl impl_;
}


*/























