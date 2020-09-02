#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#define N 64

class MD5{
public:
	// 构造函数
	MD5()
		:_chunkByte(N)
	{
		//构造函数用来初始化原始数据
		init();
		memset(_chunk, 0, _chunkByte);
		_totalByte = _lastByte = 0;

	}
	

	// 接收字符串
	std::string StrMD5(const std::string& str) {
		// 如果字符串为空直接返回空串
		if (str.empty()) {
			return"";
		}
		else {
			unsigned char* pstr = (unsigned char*)str.c_str();
			//把字符串传换成字符类型的数组
			size_t num_Chunk = str.size() / _chunkByte;
			for (size_t i = 0; i < num_Chunk; ++i) {
				_totalByte += _chunkByte;
				calculateMD5((size_t*)pstr + i * _chunkByte);
			}

			_lastByte = str.size() % _chunkByte;
			memcpy(_chunk, pstr + _totalByte, _lastByte);
			calculateMD5Final();
		}
		return changeHex(_a) + changeHex(_b) + changeHex(_c) + changeHex(_d);
	}
private:
	void init(){
		// 初始化 ABCD
		_a = 0x67452301;
		_b = 0xefcdab89;
		_c = 0x98badcfe;
		_d = 0x10325476;

		//用来记录每次循环左移所需要移动的位数
		size_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
			12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
			4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
			15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
		// TODO

		memcpy(_sft, s, sizeof(s));
		// k[i] 2^32 * abs(sin(i))
		for (int i = 0; i < 64; ++i) {
			_k[i] = (size_t)(pow(2, 32)*abs(sin(i + 1)));
		}
	}


	//处理最后一块数据段的方式
	void calculateMD5Final() {
		//函数用于对整体文件数据的最后位数进行填充
		// lastByte： 小于64个字节，表示最后一块数据大小
		unsigned char* p = _chunk + _lastByte;
		//p记录需要填充的起始位置
		//填充位的前八位 : 1000 0000 0x80 
		*p++ = 0x80;
		size_t remainFillByte = _chunkByte - _lastByte - 1;//判断需要填充的位数
		// 剩余填充字节数 小于8 个字节
		if (remainFillByte < 8) {
			// 从下一个自己二开始 全部填 0
			memset(p, 0, remainFillByte);
			calculateMD5((size_t*)_chunk);
			memset(_chunk, 0, _chunkByte);
			//将新出现在空间的中的512位全部置为0
			//最后将数组中的后8位置为文章长度
		}
		else {
			// 填充 0 后 剩余字节数 大于等于 8 个字节  8个字节前 中间全部填 0 
			memset(p, 0, remainFillByte);
		}
		// 最后的64 bit 存放原始文档的bit长度

		// char 类型 最大范围 256 最终类型 还是要以long long 类型存放
		//_chunk = _totalByte * 8;
		((unsigned long long*) _chunk)[7] = _totalByte * 8;
		//最后的字节存放文档的长度
		calculateMD5((size_t*)_chunk);

	}
	//进行64次操作，每一轮处理四个字节(一个数据块)
	std::string changeHex(size_t num) {
		//用来将对应的整形数组转成16进制字符串形式
		static std::string strMap = "0123456789abcdef";
		std::string ret;
		std::string byteStr;
		for (int i = 0; i < 4; ++i) {
			byteStr = "";
			size_t b = (num >> (i * 8)) & 0xff;
			for (int j = 0; j < 2; ++j) {
				byteStr.insert(0, 1, strMap[b % 16]);
				b /= 16;
			}
			ret += byteStr;
		}
		return ret;
	}



	size_t F(size_t x, size_t y, size_t z) {
		return (x & y) | ((~x) & z);
	}
	size_t G(size_t x, size_t y, size_t z) {
		return (x & z) | (y & (~z));
	}
	size_t H(size_t x, size_t y, size_t z) {
		return x ^ y ^ z;
	}
	size_t I(size_t x, size_t y, size_t z) {
		return y ^ (x | (~z));
	}

	size_t shiftLeftRotate(size_t num, size_t n) {
		return (num << n) | (num >> (32 - n));
		//循环左移n位
	}

private:
	// 经过 64轮操作
	//每次操作处理 4 个字节
	void calculateMD5(size_t* chunk) {
		//建立大体框架
		//函数用于将所得的内容数据块进行分批次操作

		size_t a = _a;
		size_t b = _b;
		size_t c = _c;
		size_t d = _d;
		// chunk[g]
		// f 是 哈希函数返回值 
		size_t f, g;

		// 64 次变换， 4轮操作，每一轮操作：16个子操作

		for (size_t i = 0; i < 64; ++i) {
			/*
			if (0 <= i < 16) g = i;
			if (16 <= i < 32) g = (5 * i + 1) % 16;
			if (32 <= i < 48) g = (3 * i + 5) % 16;
			if(48 <= i < 63) g = (7 * i) % 16;*/

			/*
			F(x,y,z) = (x & y) | ((~x) & z)
			G(x,y,z) = (x & z) | ( y & (~z))
			H(x,y,z) = x ^ y ^ z
			I(x,y,z) = y ^ (x | (~z))
			*/
			if (0 <= i && i < 16) {
				f = F(b, c, d);
				g = i;
			}
			else if (16 <= i && i < 32) {
				f = G(b, c, d);
				g = (5 * i + 1) % 16;
			}
			else if (32 <= i && i < 48) {
				f = H(b, c, d);
				g = (3 * i + 5) % 16;
			}
			else {
				f = I(b, c, d);
				g = (7 * i) % 16;
			}

			//更新数据块中的值
			size_t dtmp = d;
			d = c;
			c = b;
			b = b + shiftLeftRotate(a + f + _k[i] + chunk[g], _sft[i]);
			//f哈希函数的返回值,_chunk[g]当前用到的数据块，
			//_sft[i] 循环左移的位数
			a = dtmp; //a=没有更新前的d（dtmp暂存之前的d值)
		}

		_a += a;
		_b += b;
		_c += c;
		_d += d;
	}

private:
	size_t _a;
	size_t _b;
	size_t _c;
	size_t _d;

	size_t _k[N];//存放64个值
	size_t _sft[N];//循环移位(决定每次移动的位数)
	size_t _lastByte;//最后一次读取的数据块大小

	const size_t _chunkByte; //每一个运算
	unsigned char _chunk[N];  //512位二进制位 
	unsigned long long _totalByte;
};