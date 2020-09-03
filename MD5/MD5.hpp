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
	MD5();
	// 接收文件
	std::string getFiledMDd5(const char* filename);
	// 接收字符串
	std::string getStringMD5(const std::string& str);
private:
	void init();

	void calculateMD5Final();//处理最后一块数据段的方式
	//进行64次操作，每一轮处理四个字节(一个数据块)
	std::string changeHex(size_t num);


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

	// 经过 64轮操作
	//每次操作处理 4 个字节
	void calculateMD5(size_t* chunk);

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
	unsigned long long _totalByte ;
};

