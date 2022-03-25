#pragma once
#include <memory>
#include <string>
#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"



class LogFile : noncopyable
{
public:
	//每被appendflushEveryN次，flush一次，flush往文件中写，只不过文件也设置了缓冲区
	LogFile(const string& basename,int flushEveryN = 1024);
	~LogFile();

	void append(const char* logline,int len);
	void flush();//清空缓冲区
	void rollFile();//滚动日志

private:
	void append_unlocked(const char* logline,int len);
	const std::string basename_;//日志文件basename
	const int flushEveryN;//日志写入间隔次数

	int count_;
	//计数器，当它的值达到FlushEveryN时，就去检测是否需要滚动日志或写入文件
	std::unique_ptr<MutexLock> mutex_;
	std::unique_ptr<AppendFile> file_;
};



























