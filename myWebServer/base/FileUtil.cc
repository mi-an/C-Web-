#include "FileUtil.h"
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


using namespace std;


AppendFile::AppendFile(string filename)
	: fp_(fopen(filename.c_str(),"ae"))
{
	//自己提供缓冲区,当缓冲区满了，会自动flush
	setbuffer(fp_,buffer_,sizeof buffer_);
}

AppendFile::~AppendFile() { fclose(fp_); }

void AppendFile::append(const char* logline,const size_t len)
{
	size_t n = this->write(logline,len);
	size_t remain = len-n;
	while(remain>0)
	{
		size_t x = this->write(logline+n,remain);
		if(x==0)
		{
			int err = ferror(fp_);
			if(err)//异常导致的写入失败，退出
			{
				fprintf(stderr,"AppendFile::Append() failed !\n");
				break;
			}
			//否则是信号中断的，继续写
			n+=x;
			remain = len-n;
		}
	}
}

void AppendFile::flush() { fflush(fp_); }

size_t AppendFile::write(const char* logline,size_t len)
{
	//采用不加锁的方式写入，更加高效
	return fwrite_unlock(logline,1,len,fp_);
}
