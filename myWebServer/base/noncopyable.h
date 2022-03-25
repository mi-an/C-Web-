#pragma once


class noncopyable
{
protected:
	noncopyable() {}
	~noncopyable() {}
private:
	//禁止了拷贝与赋值操作
	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&);
};





