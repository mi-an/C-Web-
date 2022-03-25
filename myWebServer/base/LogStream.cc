







//template<int SIZE>
//const char* FixedBuffer<SIZE>::debugString()
//{
//	*cur_ = '\0';
//	return data_;
//}


//这些类型的输出都是按照int类型来处理的，比int小的转化成int，比int大的直接进行格式化
LogStream& LogStream::operator<<(short v)
{
	*this << static_cast<int>(v);
	return *this;
}
LogStream& LogStream::operator<<(unsigned short v)
{
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
	formatInteger(v);
	return *this;
}


LogStream& LogStream::operator<<(long long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
	formatInteger(v);
	return *this;
}


LogStream& LogStream::operator<<(double v)
{
	if(avail()>=kMaxNumbericSize)
	{
		int len = snprintf(buffer_.current(),kMaxNumbericSize,"%.12g",v);
		buffer_.add(len);
	}
	return *this;
}

LogStream& LogStream::operator<<(long double v)
{
	if(avail()>=kMaxNumbericSize)
	{
		int len = snprintf(buffer_.current(),kMaxNumbericSize,"%.12Lg",v);
		buffer_.add(len);
	}
	return *this;
}

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
//BOOST_STATIC_ASSERT(sizeof(digits) == 20);

const char digitHex[] = "0123456789ABCDEF";
BOOST_STATIC_ASSERT(sizeof digitsHex == 17);

template<typename T>
size_t convert(char buf[],T value)
{
	T i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i%10);
		i/=10;
		*p++ = zero[lsd];//相当于*(zero+lsd),zero指向digits数组首元素
	}while(i!=0)

	if(value < 0)
	{
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf,p);
	return p-buf;
}



//格式化整型，所有类型都是按照int类型格式化的
template<typename T>
void LogStream::formatInteger(T v)
{
	//剩余空间>=32字节,buffer容不下32字节的话，直接丢弃
	if(buffer_.avail() >= kMaxNumbericSize)
	{
		size_t len = convent(buffer_.current(),v);//把整数转换成字符串形式存放到buffer
		buffer_.add(len);
	}
}



LogStream& LogStream::operator<<(const void* p)
{
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	if(buffer_.avail() >= kMaxNumbericSize)
	{
		char* buf = buffer_.current();
		buf[0] = '0';
		buf[1] = 'x';
		size_t len = convertHex(buf+1,v);
		buffer_.add(len+2);
	}
	return *this;
}


//uintptr_t 对于32为平台来说就是unsigned int
//对于64为平台来说就是unsigned long int
size_t convertHex(char buf[],uintptr_t value)
{
	uintptr_t i = value;
	char* p = buf;

	do
	{
		int lsd = i%16;
		i/=16;
		*p++ = digitsHex[lsd];
	}while(i!=0);

	*p = '\0';
	std::reverse(buf,p);
	return p-buf;
}




/*
template<typename T>
Fmt::Fmt(const char* fmt,T val)
{
	//断言T是算术类型
	BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);

	length_ = snprintf(buf_,sizeof buf_,fmt,val);
	assert(static_cast<size_t>(length_) < sizeof buf_);
}
//特化版本
template Fmt::Fmt(const char* fmt,char);
template Fmt::Fmt(const char* fmt,short);
template Fmt::Fmt(const char* fmt,unsigned short);
template Fmt::Fmt(const char* fmt,int);
template Fmt::Fmt(const char* fmt,unsigned int);
template Fmt::Fmt(const char* fmt,long long);
template Fmt::Fmt(const char* fmt,unsigned long long);


template Fmt::Fmt(const char* fmt,float);
template Fmt::Fmt(const char* fmt,double);
*/










