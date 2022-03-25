#pragma once
#include <assert.h>
#include <string.h>
#include <string>
#include "noncopyable.h"

class AsyncLogging;
const int kSmallBuffer = 4*1000;//一个缓冲区的大小，4K
const int kLargeBuffer = 4*1000*1000;//4M

template<int SIZE>
class FixedBuffer : noncopyable
{
public:
	FixedBuffer() : cur_(data_) {}

	~FixedBuffer() {}

	void append(const char* buf,size_t len)
	{
		if(avail()>static_cast<int>(len))
		{
			memcpy(cur_,buf,len);
			cur_ += len;
		}
	}

	const char* data() const { return data_; }
	int length() const { return static_cast<int>(cur_-data_); }

	char* current() { return cur_; }
	int avail() const { return static_cast<int>(end() - cur_); }
	void add(size_t len) { cur_ += len; }

	void reset() { cur_ = data_; }
	void bzero() { memset(data_,0,sizeof data_); }
private:
	const char* end() const { return data_ + sizeof data_; }

	char data_[SIZE];
	char* cur_;
};

class LogStream : noncopyable
{
public:
	typedef LogStream self;

	typedef FixedBuffer<kSmallBuffer> Buffer;

	self& operator<<(bool v)
	{
		buffer_.append(v ? "1" : "0" , 1);
		return *this;
	}

	self& operator<<(short);
	self& operator<<(unsigned short);
	self& operator<<(int);
	self& operator<<(unsigned int);
	self& operator<<(long);
	self& operator<<(unsigned long);
	self& operator<<(long long);
	self& operator<<(unsigned long long);

	//把地址存放进去，以16进制形式
	self& operator<<(const void*);

	self& operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}
	self& operator<<(double);

	self& operator<<(char v)
	{
		buffer_.append(&v,1);
		return *this;
	}

	LogStream& operator<<(const char* str)
	{
		if(str)
		{
			buffer_.append(str,strlen(str));
		}
		else
		{
			buffer_.append("(null)",6);
		}
		return *this;
	}

	LogStream& operator<<(const unsigned char* str)
	{
		return operator<<(reinterpret_cast<const char*>(str));
	}

	LogStream& operator<<(const std::string& v)
	{
		buffer_.append(v.c_str(),v.size());
		return *this;
	}

	void append(const char* data,int len) { buffer_.append(data,len);}
	const Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }
private:
	void staticCheck();

	template<typename T>
	void formatInteger(T);

	Buffer buffer_;

	static const int kMaxNumbericSize = 32;

}


/*


namespace muduo
{

namespace detail
{
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;


//缓冲区类
template<int SIZE>
class FixedBuffer : boost::noncopyable
{
public:
	FixedBuffer()
		: cur_(data_)
	{
		setCookie(cookieStart);
	}
	~FixedBuffer()
	{
		setCookie(cookieEnd);
	}


	//向buffer中写入数据
	void append(const char* buf,size_t len)
	{
		if(implicit_cast<size_t>(avail()) > len)
		{
			memcpy(cur_,buf,len);
			cur_ += len;
		}
	}

	
	const char* data() const { return data_; }
	int length() const { return static_cast<int>(cur_ - data_); }
	//当前缓冲区中数据长度
	char* current() { return cur_; }
	int avail() const { return static_cast<int>(end()-cur_); }
	//返回当前buffer的可用空间
	void add(size_t len) { cur_ += len; }

	//空间被重置 
	void reset() { cur_ = data_; }
	void bzero() { ::bzero(data_,sizeof data_); }

	//仅仅used for GDB
	const char* debugString();//在数据末尾加一个\0，返回回来

	void setCookie(void (*cookie)()) { cookie_ = cookie; }

	string asString() const { return string(data_,length()); }
private:
	const char* end() const { return data_ + sizeof data_; }
	//返回整个buffer缓冲区的end指针
	static void cookieStart();
	static void cookieEnd();

	void (*cookie)();//函数指针,在构造函数中传入cookiestart；析构中传入cookiend；
	//当前没有什么用
	char data_[SIZE];
	char* cur_;//指向写入buffer位置
};

}

class LogStream : boost::noncopyable
{
	typedef LogStream self;
public:
	typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

	self& operator<<(bool v)
	{
		buffer_.append(v ? "1" : "0" , 1);
		return *this;
	}

	self& operator<<(short);
	self& operator<<(unsigned short);
	self& operator<<(unsigned int);
	self& operator<<(long);
	self& operator<<(unsigned long);
	self& operator<<(long long);
	self& operator<<(unsigned long long);

	//把地址存放进去，以16进制形式
	self& operator<<(const void*);

	self& operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}
	self& operator<<(double);

	self& operator<<(char v)
	{
	}

	self& operator<<(const string& v)
	{
		buffer_.append(v.c_str(),v.size());
	}

	void append(const char* data,int len) { buffer_.append(data,len); }
	const Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }
private:
	void staticCheck();

	template<typename T>
	void formatInteger(T);

	Buffer buffer_;

	static const int kMaxNumbericSize = 32;
};



class Fmt
{
public:
	//把一个整数格式化成fmt的格式，存储到buf
	template<typename T>
	Fmt(const char* fmt,T val);

	const char* data() const { return buf_; }
	int length() const { return length_; }

private:
	char buf_[32];
	int length;
};
inline LogStream& operator<<(LogStream& s,const Fmt& fmt)
{
	s.append(fmt.data(),fmt.length());
	return s;
}


}

*/



