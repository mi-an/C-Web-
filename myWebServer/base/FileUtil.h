#pragma once
#include <string>
#include <noncopyable.h>

//这个类并不是线程安全的，LogFile是线程安全的，而这个类是在LogFile中使用的
//所以不用实现成线程安全的，也可以
class AppendFile : noncopyable
{
public:
	explicit AppenfFile(std::string filename);
	~AppendFile();
	//append会向文件写
	void append(const char* logline,const size_t len);
	void flush();
private:
	size_t write(const char* logline,size_t len);
	FILE* fp_;//文件指针
	char buffer_[64*1024];//文件指针的缓冲区
};







